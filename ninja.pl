#!/usr/bin/perl -w

use strict;
use warnings;

my $input_file = 'output.txt';  # 替换为你的输入文件路径
my $output_file = 'build.ninja';  # Ninja 构建文件的输出路径

open my $in_fh, '<', $input_file or die "无法打开文件 $input_file: $!";

open my $out_fh, '>', $output_file or die "无法打开文件 $output_file: $!";

# Ninja 文件的基础结构
print $out_fh "rule rsync\n";
print $out_fh "  command = rsync --mkpath \$in \$out\n";
print $out_fh "  description = Syncing \$in to \$out\n";

# 为每个输入文件生成 Ninja 规则
while (my $line = <$in_fh>) {
    chomp $line;  # 移除行尾的换行符
    my $src_path = "hd/$line";
    my $dest_path = "patch/$line";
    print $out_fh "build $dest_path: rsync $src_path\n";
}

close $in_fh;
close $out_fh;

print "Generated $output_file.\n";

