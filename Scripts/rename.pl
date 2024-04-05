#!/usr/bin/perl

use strict;
use warnings;

# Function to recursively process files in a directory
sub recursive_replace {
    my ($dir) = @_;
    opendir(my $dh, $dir) or die "Could not open directory '$dir': $!";
    while (my $file = readdir($dh)) {
        next if $file eq '.' or $file eq '..';
        my $path = "$dir/$file";
        if (-d $path) {
            print "Processing directory $path ...\n";
            recursive_replace($path);
        } elsif (-f $path && ($file =~ /\.cpp$/ || $file =~ /\.h$/)) {
            print "Processing file $path ...\n";
            my $content = do {
                local $/;
                open my $fh, '<', $path or die "Could not open '$path' for reading: $!";
                <$fh>;
            };
            my $num_changes = 0;
            $num_changes += $content =~ s/Hazel/Snowstorm/g;
            $num_changes += $content =~ s/HZ/SS/g;
            $num_changes += $content =~ s/hzpch/pch/g;
            if ($num_changes) {
                open my $fh, '>', $path or die "Could not open '$path' for writing: $!";
                print $fh $content;
                close $fh;
                print "Number of changes applied: $num_changes\n";
            } else {
                print "No changes needed in $path\n";
            }
        }
    }
    closedir $dh;
}

# Call the function starting from the current directory
recursive_replace('.');