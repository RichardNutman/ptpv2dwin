#**************************************
#
#Copyright notice:
#
#Author: Alan K. Bartky, Bartky Networks, 
#        alan@bartky.net, www.bartky.net
#
#Copyright (c) 2007 Alan K. Bartky
#
#Permission is hereby granted to use, copy, modify, and distribute this software
#for any purpose and without fee, provided that this notice appears in all
#copies of this file and any derivative works. 
#
#The author makes no representations about the suitability of this
#software for any purpose. This software is provided "as is" without express or
#implied warranty.
#
#***************************************
%define pfx /opt/freescale/rootfs/%{_target_cpu}

Summary         : The PTP daemon which implements the IEEE 1588v1 Precision Time protocol (PTP)
Name            : ptpd
Version         : 1rc1
Release         : RC1
License         : BSD
Vendor          : Bartky Networks (www.bartky.net)
Packager        : Alan K. Bartky (alan@bartky.net)
Group           : System Environment/Daemons
URL             : http://sourceforge.net/projects/ptpd/
Source          : %{name}-%{version}.tar.gz
Patch0          : %{name}-%{version}-mpc831x.patch
BuildRoot       : %{_tmppath}/%{name}
Prefix          : %{pfx}

%Description
%{summary}

%Prep
%setup 
%patch0 -p1

%Build
cd src
make all
#./configure --prefix=%{_prefix} --host=$CFGHOST --build=%{_build}

%Install
rm -rf $RPM_BUILD_ROOT
#make install DESTDIR=$RPM_BUILD_ROOT/%{pfx}
for i in usr/sbin 
do
    mkdir -p $RPM_BUILD_ROOT/%{pfx}/$i
done
cp -a src/ptpd $RPM_BUILD_ROOT/%{pfx}/usr/sbin

%Clean
rm -rf $RPM_BUILD_ROOT

%Files
%defattr(-,root,root)
%{pfx}/*
