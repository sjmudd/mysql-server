FROM oraclelinux:9
LABEL maintainer = "Simon J Mudd <simon.mudd@booking.com>"

#
# setup the minimal install required to build MySQL on OracleLinux 9
#

RUN yum update -y && \
    yum install -y 'dnf-command(config-manager)' && \
    yum config-manager --set-enabled ol9_codeready_builder && \
    yum install -y \
	bind-utils \
	bison \
	cmake \
	cyrus-sasl-devel \
	git \
	krb5-devel \
	libaio-devel \
	libcurl-devel \
	libfido2-devel \
	libtirpc-devel \
	libudev-devel \
	ncurses-devel \
	numactl-devel \
	openldap-devel \
	openssl-devel \
	perl \
	perl-JSON rpcgen \
	rpm-build \
	time \
	gcc-toolset-12-annobin-annocheck \
	gcc-toolset-12-annobin-plugin-gcc \
	gcc-toolset-12-binutils  \
	gcc-toolset-12-dwz \
	gcc-toolset-12-gcc \
	gcc-toolset-12-gcc-c++ \
	wget \
	zlib-devel

WORKDIR /workspace/mysql-server
