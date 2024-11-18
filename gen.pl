#!/usr/bin/perl -w

use strict;
use warnings;

my $input_file = 'output.txt';  # 替换为你的输入文件路径
my $output_file = 'files.sh';  # 替换为你的输出文件路径

open my $in_fh, '<', $input_file or die "无法打开文件 $input_file: $!";

open my $out_fh, '>', $output_file or die "无法打开文件 $output_file: $!";

#while (my $line = <$in_fh>) {
#    chomp $line;  # 移除行尾的换行符
#    print $out_fh "cp cn/$line patch\n";  # 在行首添加 cp，在行尾添加 .
#}
while (my $line = <$in_fh>) {
    chomp $line;  # 移除行尾的换行符
    my $src_path = "cn/$line";
    my $dest_path = "patch/$line";
    print $out_fh "rsync --mkpath $src_path $dest_path\n";  # 生成 cp 命令并写入输出文件
}

close $in_fh;
close $out_fh;

print "Generuated $output_file.\n";
