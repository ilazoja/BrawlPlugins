#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

export TOOLS 	:= $(CURDIR)/tools
export LIB 		:= $(CURDIR)/lib


.PHONY: all AsyncRSP CodeMenu ftp NetLog Physics Sandbox MenuTest pplusRecords

all: AsyncRSP CodeMenu ftp NetLog Physics Sandbox MenuTest pplusRecords

AsyncRSP:
	@cd "AsyncRSP" && make clean && make

CodeMenu:
	@cd "CodeMenu" && make clean && make

ftp:
	@cd "ftp" && make clean && make

NetLog:
	@cd "NetLog" && make clean && make

Physics:
	@cd "Physics" && make clean && make

Sandbox:
	@cd "Sandbox" && make clean && make

MenuTest:
	@cd "MenuTest" && make clean && make

pplusRecords:
	@cd "pplusRecords" && make clean && make

clean:
	@cd "AsyncRSP" && make clean
	@cd "CodeMenu" && make clean
	@cd "ftp" && make clean
	@cd "NetLog" && make clean
	@cd "Physics" && make clean
	@cd "Sandbox" && make clean
	@cd "MenuTest" && make clean
	@cd "pplusRecords" && make clean
