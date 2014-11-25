#! /usr/bin/ruby

# Copyright 2002 Neil Spring <nspring@cs.washington.edu>
# GPL
# report bugs to wmbiff-devel@lists.sourceforge.net
# or (preferred) use the debian BTS via 'reportbug'

# Based on security-update-check.py by Rob Bradford

require 'net/ftp'

#require 'profile'

# re-fetch interval - only bug the server once every hour.
# allows wmbiff to ask us often how many packages have been
# updated so that the number goes back to cyan (old) from
# yellow (new) quickly on upgrade.

# this still doesn't mean we grab the whole file.  we get
# if-modified-since.  it just means we don't connect to the
# server more often than this.
# 6 hours * 60 min/hour * 60 sec/min
Refetch_Interval_Sec = 6 * 60 * 60

# as an ordinary user, we store Packages in the home directory.
Cachedir = ENV['HOME'] + '/.wmbiff-sdr'

# look for updates from this server.  This script is designed around
# (and simplified greatly by) using just a single server.
Server = 'security.debian.org'

# extend the Array class with a max method.
class Array
  def inject(n)
    each { |value| n = yield(n, value) }
    n
  end
  def max
    inject(0) { |n, value| ((n > value) ? n : value) }
  end
end

def debugmsg(str)
  $stderr.puts str if($VERBOSE)
end

# to be reimplemented without execing touch.
def touch(filename)
  debugmsg "touching #{filename}"
  Kernel.system('/usr/bin/touch ' + filename)
end

# to be reimplemented without execing dpkg, though running
# dpkg excessively doesn't seem to be a bottleneck.
def version_a_gt_b(a, b)
  cmd = "/usr/bin/dpkg --compare-versions %s le %s" % [ a, b ]
  # $stderr.puts cmd
  return (!Kernel.system(cmd))
end

# figure out which lists to check
# there can be many implementations of
# this behavior, this seemed simplest.


# we're going to make an array of arrays, for each package
# file, the url, the system's cache of the file, and a
# per-user cache of the file.
packagelists = Dir.glob("/var/lib/apt/lists/#{Server}*Packages").map { |pkgfile|
  [ '/debian-security' + pkgfile.gsub(/.*#{Server}/, '').tr('_','/').gsub(/Packages/, ''), # the url path
    pkgfile,  # the system cache of the packages file.  probably up-to-date.
    # and finally, a user's cache of the page, if needed.
    "%s/%s" % [ Cachedir, pkgfile.gsub(/.*#{Server}_/,'') ]
  ]
}

# update the user's cache if necessary.
packagelists.each { |urlpath, sc, uc|
  sctime = File.stat(sc).mtime
  cached_time =
    if(test(?e, uc)) then
      uctime = File.stat(uc).mtime
      if ( uctime < sctime ) then
        # we have a user cache, but it is older than the system cache
        File.unlink(uc)  # delete the obsolete user cache.
        sctime
      else
        uctime
      end
    else
      sctime
    end
  if(Time.now > cached_time + Refetch_Interval_Sec) then
    debugmsg "fetching #{urlpath} %s > %s + %d" % [Time.now, cached_time, Refetch_Interval_Sec]
    begin
      test(?e, Cachedir) or Dir.mkdir(Cachedir)

      ftp = Net::FTP.new(Server)
      ftp.login
      ftp.chdir(urlpath)
      ftp.getbinaryfile('Packages.gz', uc + '.gz', 1024)
      ftp.close

      # need to unzip Packages.gz
      cmd_gunzip = "gzip -df %s" % [ uc + '.gz' ]
      Kernel.system(cmd_gunzip)

      rescue SocketError => e
        # if the net is down, we'll get this error; avoid printing a stack trace.
        puts "XX old"
        puts e
        exit 1;
      rescue Timeout::Error => e
        # if the net is down, we might get this error instead.
        # but there is no good reason to print the specific exception. (execution expired)
        puts "XX old"
        exit 1;
      end
      debugmsg "urlpath updated"
  else
    debugmsg "skipping #{urlpath}"
  end
}

available = Hash.new
package = nil
packagelists.each { |url, sc, uc|
  File.open( (test(?e, uc)) ? uc : sc, 'r').each { |ln|
    if(m = /^Package: (.*)/.match(ln)) then
      package = m[1]
    elsif(m = /^Version: (.*)/.match(ln)) then
      available[package] = m[1]
    end
  }
}

installed = Hash.new
package = nil
isinstalled = false
File.open('/var/lib/dpkg/status').each { |ln|
  if(m = /^Package: (.*)$/.match(ln)) then
    package = m[1]
    isinstalled = false # reset
  elsif(m = /^Status: install ok installed/.match(ln)) then
    isinstalled = true
  elsif(m = /^Version: (.*)$/.match(ln)) then
    isinstalled && installed[package] = m[1]
  end
}

debugmsg "%d installed, %d available" % [ installed.length, available.length ]

updatedcount = 0
updated = Array.new
( installed.keys & available.keys ).each { |pkg|
  if(version_a_gt_b(available[pkg], installed[pkg])) then
    updatedcount += 1
    updated.push(pkg + ": #{available[pkg]} > #{installed[pkg]}")
  end
}

# we're done.  output a count in the format expected by wmbiff.
if(updatedcount > 0) then
  puts "%d new" % [ updatedcount ]
else
  puts "%d old" % [ installed.length ]
end

puts updated.join("\n")
