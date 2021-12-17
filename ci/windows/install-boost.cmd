curl -o boost.zip -kLO https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_1_78_0.zip -f --retry 5 -C -
7z x boost.zip -oboost_install
cd boost_install/boost_1_78_0
bootstrap.bat
b2.exe install prefix=cmbuild/boost