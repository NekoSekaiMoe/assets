#!/usr/bin/perl
use strict;
use warnings;

# 输入文件名
my $input_file = 'input.txt';
open(my $in, '<', $input_file) or die "无法打开文件 $input_file: $!";

# 输出文件名
my $output_file = 'output.txt';
open(my $out, '>', $output_file) or die "无法创建文件 $output_file: $!";

# 逐行处理
while (my $line = <$in>) {
    chomp($line);
    # 匹配包含路径的行
    if ($line =~ m{二进制文件 (.+) 和 (.+) 不同}) {
        my ($path1, $path2) = ($1, $2);

        # 提取公共部分路径 PSP_GAME/USRDIR/...
        if ($path1 =~ m{(PSP_GAME/USRDIR/.*)}) {
            print $out "$1\n";
            print "提取并保存路径: $1\n";  # 调试信息
        }
    }
}

# 关闭文件句柄
close($in);
close($out);

print "提取完成，结果已保存到 $output_file\n";

