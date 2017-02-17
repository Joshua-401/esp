# Copyright 2017 Columbia University, SLD Group

############################################################
# Design Parameters
############################################################

set MODULE sort

#
# Source the common configurations
#
source ../../common/stratus/project.tcl


#
# Testbench or system level modules
#
#define_system_module main ../tb/sc_main.cpp
define_system_module tb32 ../tb/system.cpp ../tb/sc_main.cpp

define_system_config tb32 TESTBENCH32  -DDMA_WIDTH_VAL=32

#
# System level modules to be synthesized
#
define_hls_module sort32 ../src/$MODULE.cpp --template "$MODULE<32>"

#
# HLS Configuration (associated with SC_MODULEs)
#
define_hls_config sort32 BASIC32 --clock_period=$CLOCK_PERIOD $COMMON_HLS_FLAGS


######################################################################
# Simulation configurations
######################################################################


set TESTBENCHES {{32 1} }
append TESTBENCHES {{32 4} }
append TESTBENCHES {{64 8} }
append TESTBENCHES {{256 16} }
append TESTBENCHES {{1024 1204} }

foreach tb $TESTBENCHES {

    set test [lindex $tb 0]_[lindex $tb 1]
    set ARGV ""
    append ARGV [lindex $tb 0];  # argv[1]
    append ARGV " "
    append ARGV [lindex $tb 1];  # argv[2]
    define_sim_config "BEHAV_$test" "sort32 BEH" {tb32 TESTBENCH32} -argv $ARGV

}

define_sim_config "BEHAV_default" "sort32 BEH" {tb32 TESTBENCH32} -argv "64 8"

#
# The following rules are TCL code to create a simulation configuration
# for both RTL_C and RTL_V for each hls_config defined
#
foreach tb $TESTBENCHES {
    set test [lindex $tb 0]_[lindex $tb 1]
    set ARGV ""
    append ARGV [lindex $tb 0] ;  # argv[1]
    append ARGV " "
    append ARGV [lindex $tb 1] ;  # argv[2]

    foreach config [find -hls_config *] {
        set cname [get_attr name $config]
	if {$TECH_IS_XILINX == 1} {
	    define_sim_config "$cname\_$test\_V" "sort32 RTL_V $cname" {tb32 TESTBENCH32} -argv $ARGV -verilog_top_modules glbl
	} else {
	    define_sim_config "$cname\_$test\_V" "sort32 RTL_V $cname" {tb32 TESTBENCH32} -argv $ARGV
	}
    }
}

if {$TECH_IS_XILINX == 1} {
    define_sim_config "RTL_V_default" "sort32 RTL_V BASIC32" {tb32 TESTBENCH32} -argv "64 8" -verilog_top_modules glbl
} else {
    define_sim_config "RTL_V_default" "sort32 RTL_V BASIC32" {tb32 TESTBENCH32} -argv "64 8"
}

#
# Compile Flags
#
set_attr hls_cc_options "$INCLUDES"

#
# Simulation Options
#
use_systemc_simulator incisive
set_attr cc_options "$INCLUDES -DCLOCK_PERIOD=$CLOCK_PERIOD"
# enable_waveform_logging -vcd
set_attr end_of_sim_command "make saySimPassed"