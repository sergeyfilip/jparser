Name: jparser
Version: 2.1.1
Release: 0
Summary: ja3 fingerprints creator and parser tools
Group: System Environment/Base
License: GPL
Source0: %{name}-%{version}.tar.gz
BuildArch: x86_64
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: jsoncpp-devel jsoncpp
BuildRequires: libconfig-devel
Requires: libconfig

%define _binaries_in_noarch_packages_terminate_build   0
%define debug_package %{nil}

%description
ja3 fingerprints rules creator and parser

%prep
%setup -q

%build
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/bin/
mkdir -p $RPM_BUILD_ROOT/etc/jparser/
mkdir -p $RPM_BUILD_ROOT/var/lib/jparser/
install jparser $RPM_BUILD_ROOT/usr/bin
install janalyzer $RPM_BUILD_ROOT/usr/bin
install jreport $RPM_BUILD_ROOT/usr/bin
install jmapper $RPM_BUILD_ROOT/usr/bin
install jgenerator $RPM_BUILD_ROOT/usr/bin
install jupdater $RPM_BUILD_ROOT/usr/bin
install jinteger $RPM_BUILD_ROOT/usr/bin
install jmaker $RPM_BUILD_ROOT/usr/bin
install stop_whitelisting $RPM_BUILD_ROOT/usr/bin
install ./data/full.json $RPM_BUILD_ROOT/etc/jparser/
install ./data/conf.cfg $RPM_BUILD_ROOT/etc/jparser/
install ./data/categories.txt $RPM_BUILD_ROOT/etc/jparser/
install ./data/categories_orig.txt $RPM_BUILD_ROOT/etc/jparser/
install ./data/ja3fingerprint.json $RPM_BUILD_ROOT/etc/jparser/
install ./data/devices.txt $RPM_BUILD_ROOT/var/lib/jparser/
install ./data/policies.json $RPM_BUILD_ROOT/var/lib/jparser/
install ./data/awl.yaml $RPM_BUILD_ROOT/var/lib/jparser/

%files
%config(noreplace) /var/lib/jparser/policies.json
%defattr(-,root,root)
/usr/bin/jparser
/usr/bin/janalyzer
/usr/bin/jreport
/usr/bin/jmapper
/usr/bin/jgenerator
/usr/bin/jupdater
/usr/bin/jinteger
/usr/bin/jmaker
/usr/bin/stop_whitelisting
/etc/jparser/full.json
/etc/jparser/conf.cfg
/etc/jparser/categories.txt
/etc/jparser/categories_orig.txt
/etc/jparser/ja3fingerprint.json
/var/lib/jparser/devices.txt
#/var/lib/jparser/policies.json
/var/lib/jparser/awl.yaml

#%exclude %dir /sources

%clean
make clean
rm -rf $RPM_BUILD_ROOT

%post
ldconfig

%preun


%changelog
* Fri Dec 18 2020 Sergey Filipovich <sergey@redpiranha.net> 2.1.1
- generation process was rebuild and improved
- new utilities added
- start/stop scripts removed

* Wed Dec 02 2020 Sergey Filipovich <sergey@redpiranha.net> 2.0.1
- install/uninstall scripts were removed
- new fingerprints database added
- start/stop logic changed

* Mon Oct 26 2020 Sergey Filipovich <sergey@redpiranha.net> 1.9.7
- stop script was changed
- cron job added

* Fri Jul 24 2020 Sergey Filipovich <sergey@redpiranha.net> 1.9.6
- new aproach in rule generation and fingerprint analysis 

* Mon Jun 8 2020 Sergey Filipovich <sergey@redpiranha.net> 1.9.4
- stop script corrected, generation code messages corrected

* Wed May 27 2020 Sergey Filipovich <sergey@redpiranha.net> 1.9.3
- Install_yaml bug corrected

* Mon Mar 30 2020 Sergey Filipovich <sergey@redpiranha.net> 1.9.2
- Update system was added

* Fri Feb 7 2020 Sergey Filipovich <sergey@redpiranha.net>
- Configuration scripts were added

* Wed Jan 29 2020 Sergey Filipovich <sergey@redpiranha.net>
- Fingerprints database was changed, some files removed

* Tue Nov 26 2019 Sergey Filipovich <sergey@redpiranha.net>
- New fingerprints database added, rulesgenerator allow bug corrected

* Thu Nov 07 2019 Sergey Filipovich <sergey@redpiranha.net>
- New config management added

* Mon Sep 23 2019 Sergey Filipovich <sergey@redpiranha.net>
- New services added with new classes - Policy, Group, ...

* Wed Jun 26 2019 Sergey Filipovich <sergey@redpiranha.net>
- Duplicated sids bug resolved.

* Wed May 15 2019 Sergey Filipovich  <sergey@redpiranha.net>
- Initial build
