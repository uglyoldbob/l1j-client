#! /usr/bin/perl -w

#created by
#thomas epperson (epperson@doors-software.com)
#www.doors-software.com
#this script is GPL

print("PATH = " . `echo $PATH` . "\n");

use Date::Format;
use Date::Manip;
use DateTime::Format::Strptime;
use Time::Piece;

$gitpath = '';
$buildpath = '';
$datetime = '';
$release_file = '';
$dist_prefix = '';
$remotename = '';

my $indx = 0;
my @lines = ();
my $tmpstr;

if ($#ARGV != 0 ) {
	print "Incorrect number of arguments ($#ARGV)\n";
	exit;
}

$CFGDIR = '/etc/git_build';
#$LOGFILE = '/var/log/git_build.log';

sub nextline
{
	if ($indx > $#lines) {
		return '';
	}
	chomp($tmpstr = $lines[$indx++]);	
	while($tmpstr eq '') {
		if ($indx > $#lines) {
			return '';
		}
		chomp($tmpstr = $lines[$indx++]);
	}
	return $tmpstr;
}

sub update_config($)
{
	open(OUT, ">$ARGV[0]") or die "unable to open $ARGV[0] for writing";
	print OUT $_[0] . "\n";
	print OUT $gitpath . "\n";
	print OUT $buildpath . "\n";
	print OUT $configcmd . "\n";
	print OUT $buildcmd . "\n";
	print OUT $release_file . "\n";
	print OUT $dist_prefix . "\n";
	print OUT $remotename . "\n";
	close OUT;
	print("Updated $ARGV[0] to $_[0]\n");
}

sub git_update
{
	my $gitlog = '';
	my $mostrecent = $datetime;
	my $do_work = 0;
	chdir ($gitpath) or die $!;;
	system("git pull");
	if ( $? ne 0 )
	{
		print "command failed: $!\n";
		$do_work = 0;
	}
	else
	{
		$gitlog = `git log --since="$datetime" | grep "Date"`;
		my $output_date;
		if ($gitlog ne '')
		{
			my @vals = split('\n', $gitlog);
			#Wed Feb 1 13:37:23 2012 -0600
			my $val = $vals[0];
			foreach my $val (@vals)
			{
				$val = substr($val, 6);
				$val =~ s/^\s+//;
				my $date_a;
				my $cdate_a = '';
				my $date_b;
				my $cdate_b = '';
				$date_a = new DateTime::Format::Strptime(pattern => '%a %B %d %H:%M:%S %Y %z', on_error => 'croak');
				$date_b = new DateTime::Format::Strptime(pattern => '%a %B %d %H:%M:%S %Y %z', on_error => 'croak');
		
				$cdate_a = $date_a->parse_datetime($val);
				$cdate_b = $date_b->parse_datetime($mostrecent);
				
				if (DateTime->compare($cdate_a, $cdate_b) > 0)
				{
					print("Update most recent\n");
					$mostrecent = $date_a->format_datetime($cdate_a);
					$output_date = $cdate_a;
					$do_work = 1;
				}
			}
		}
		else
		{
			$do_work = 0;
			print "Repository not updated since $datetime\n";
		}
	}
	
	if ($do_work ne 0)
	{
		chdir($buildpath) or die $!;
		print("CURRENTLY AT: ");
		system("pwd");
		system($configcmd);
		if ( $? ne 0 )
		{
			print "command failed: $!\n";
		}
		else
		{
			system("make clean");
			system($buildcmd);
			if ( $? ne 0 )
			{
				print "command failed: $!\n";
			}
			else
			{	
				my $new_name = '';

				my $build_day = new DateTime::Format::Strptime(pattern => '%d%B%Y') or die $!;
				$new_name = $dist_prefix;
				$new_name .= $build_day->format_datetime(DateTime->now);
				$new_name .= ".tar.gz";
				system("cp $release_file $new_name\n");
				print("Upload file\n");
				
				my $command = "scp $new_name $remotename";
				print("$command\n");
				system($command);
				if ( $? ne 0 )
				{
					print "upload failed: $!\n";
				}
				else
				{
					update_config($mostrecent);
					print("File $new_name was uploaded\n");
				}
			}
		}
	}
	else
	{
		print("Nothing to do\n");
	}
}


my $sfile = $ARGV[0];

# the format of the secrets file is:
#
# date time stamp for last build

# location of git repository for autobuild (only this script should touch this directory)

# the relative path to go to for building

# the command to use for configuring

# the command to use for building

# the file for download relative to the build directory

# the prefix for the file that will be uploaded

# the username to use for scp transfer

# the path to upload the file to with scp


open(IN, "<$sfile") or die "unable to open $sfile";
@lines = <IN>;
close(IN);

# get the router status url, userid, and password

$tmpstr = nextline();
($datetime) = $tmpstr;

$tmpstr = nextline();
($gitpath) = $tmpstr;

$tmpstr = nextline();
($buildpath) = $tmpstr;

$tmpstr = nextline();
($configcmd) = $tmpstr;

$tmpstr = nextline();
($buildcmd) = $tmpstr;

$tmpstr = nextline();
($release_file) = $tmpstr;

$tmpstr = nextline();
($dist_prefix) = $tmpstr;
print("BUILD: $buildpath\n");

$remotename = nextline();
print("Remote: $tmpstr\n");
#$remotename = $tempstr;
print("Remote: $remotename \n");

git_update();
