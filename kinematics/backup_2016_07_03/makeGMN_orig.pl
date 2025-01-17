#!/usr/bin/perl

## For easier debuging, require strict perl rules
use strict;

## Standard GMm configurations
my $numOfEvents = 10000000;
my $cmuQueue = "green";

my $kinNum = 0;
my $maxGenRuns = 10;  ## Generate macros/scripts for N runs per kin setting
#my @voffsetList = (0.0, 15.0, 30.0, 45.0);
my @voffsetList = (0.0);
my $macPath = "scripts/hcal_voffset";
my $rootPath = "rootfiles/hcal_voffset";
my $cmuBatchPath = "cmu_batch/hcal_voffset";
my $jlabBatchPath = "jlab_batch/hcal_voffset";
my $macroNamePrefix = "$macPath/gmn_kin";
my $visNamePrefix = "$macPath/vis_gmn_kin";
my $runMacroNamePrefix = "$macPath/run_gmn_kin";
my $rootNamePrefix = "$rootPath/gmn_kin";
my $rootGeneratedNamePrefix = "$rootPath/generated/generated_gmn_kin";
my $visRootNamePrefix = "$rootPath/vis_gmn_kin";
my $cmuBatchNamePrefix = "$cmuBatchPath/cmu_gmn_kin";
my $jlabBatchNamePrefix = "$jlabBatchPath/jlab_gmn_kin";
my $jlabBatchLogNamePrefix = "$jlabBatchPath/logs/jlab_gmn_kin";
my $guiFileName = "$macPath/gui_gmn.mac";

## First Open up the gui file
open(my $guiFH, ">", $guiFileName) or die printError($guiFileName,$!);
print $guiFH <<TillEndOf_GUI;
## Add File menu
/gui/addMenu    file  File
/gui/addButton  file  Exit "exit"

## Add Visualization menu for all kinematics
/gui/addMenu kins Kinematics

TillEndOf_GUI


open(my $gmnFH, "<kinematics/gmn.txt") or die printError("kinematics/gmn.txt",$!);
while(my $line = <$gmnFH>) {
  ## Strip end of line character
  chomp($line);
  ## Only accept lines that do not begin with a #
  if( !($line =~ /^\#/) ) {
    # Found a valid line, let's process it
    my $kin = sprintf("%02d",++$kinNum);

    my ($tmp,$q2,$beam_e,$theta_bb,$theta_sbs,$dist_bb,$dist_mag,$dist_hcal,$bdl,
      $th_min,$th_max,$ph_min,$ph_max) = split(" ",$line);
    $th_min=$theta_bb-10;
    $th_max=$theta_bb+10;

    ## Determine magnetic field using the standard 48 inch length of 48D48
    my $field_mag = $bdl/1.2192; ## Given in Tesla
    ## Round off to 2 decimal places
    $field_mag = sprintf("%.2f",$field_mag);

    print "Generating g4sbs files for Kin$kin.\n";
    foreach my $voffset (@voffsetList) {
      ## Flag that specifies we need to build the general kin files
      ## (only needed once per kinematic and voffset setting)
      my $generalKinFileNeeded = 1;
      for(my $run = 0; $run < $maxGenRuns; $run++ ) {
        my $runString = sprintf("%03d",$run);
        my $runFileComment = "$kin\_r$runString\_v$voffset";
        my $runFileSmallComment = "$kin\_r$runString\_v$voffset\_small";
        my $kinFileComment = "$kin\_v$voffset";

        ## Define some standard file names
        my $runMacroFileName = "$runMacroNamePrefix$runFileComment.mac";
        my $cmuBatchFileName = "$cmuBatchNamePrefix$runFileComment.sh";
        my $jlabBatchFileName = "$jlabBatchNamePrefix$runFileComment.xml";
        my $jlabBatchLogFilePrefix = "$jlabBatchLogNamePrefix$runFileComment";
        my $rootFileName = "$rootNamePrefix$runFileComment.root";
        my $rootFileNameSmall = "$rootNamePrefix$runFileSmallComment.root";
        my $rootGeneratedFileName = "$rootGeneratedNamePrefix$kin\_r$runString.root";
        my $macroFileName = "$macroNamePrefix$kinFileComment.mac";
        my $visFileName = "$visNamePrefix$kinFileComment.mac";
        my $visRootFileName = "$visRootNamePrefix$kinFileComment.root";

        ## Open up the corresponding files
        open(my $runMacroFH, ">",$runMacroFileName)
          or die printError($runMacroFileName,$!);
        open(my $cmuBatchFH, ">",$cmuBatchFileName)
          or die printError($cmuBatchFileName,$!);
        open(my $jlabBatchFH, ">",$jlabBatchFileName)
          or die printError($jlabBatchFileName,$!);
        ## The following files are only needed once per setting
        my $macroFH;
        my $visFH;
        if($generalKinFileNeeded) {
          open($macroFH, ">",$macroFileName)
            or die printError($macroFileName,$!);
          open($visFH, ">", $visFileName)
            or die printError($visFileName,$!);
        }

        ## CMU batch file should be executable
        chmod(0755,$cmuBatchFH);

        ## If necessary, produce the general setting files for this kin setting
        if($generalKinFileNeeded) {
          my $tmpBuffer;
          ## Produce the general macro
          open(my $inMacroTemplate, "<", "kinematics/template_gmn.mac")
            or die printError("kinematics/template_gmn.mac");
          while(<$inMacroTemplate>) {
            s/%%(\$\w+)%%/$1/eeg;
            print $macroFH $_;
          }
          close($inMacroTemplate);

          ## Now produce the vis macro
          open(my $inVisTemplate, "<", "kinematics/template_gmn_vis.mac")
            or die printError("kinematics/template_gmn_vis.mac");
          while(<$inVisTemplate>) {
            s/%%(\$\w+)%%/$1/eeg;
            print $visFH $_;
          }
          close($inVisTemplate);

          ## Add vis menu to GUI
          print $guiFH "/gui/addButton kins Kin$kin ";
          print $guiFH "\"/control/execute $visFileName\"\n";

        }
        ## Produce the run macro
        open(my $inRunMacroTemplate, "<", "kinematics/template_gmn_run.mac")
          or die printError("kinematics/template_gmn_run.mac");
        while(<$inRunMacroTemplate>) {
          s/%%(\$\w+)%%/$1/eeg;
          print $runMacroFH $_;
        }
        close($inRunMacroTemplate);

        ## Produce the CMU batch file
        open(my $inCMUBatchTemplate, "<", "kinematics/template_gmn_cmu_batch.sh")
          or die printError("kinematics/template_gmn_cmu_cmuBatch.sh");
        while(<$inCMUBatchTemplate>) {
          s/%%(\$\w+)%%/$1/eeg;
          print $cmuBatchFH $_;
        }
        close($inCMUBatchTemplate);

        ## Produce the JLab batch file
        open(my $inJLabBatchTemplate, "<", "kinematics/template_gmn_jlab_batch.xml")
          or die printError("kinematics/template_gmn_jlab_batch.xml");
        while(<$inJLabBatchTemplate>) {
          s/%%(\$\w+)%%/$1/eeg;
          print $jlabBatchFH $_;
        }
        close($inJLabBatchTemplate);


        ## Close all open files
        close($runMacroFH);
        close($cmuBatchFH);
        close($jlabBatchFH);
        if($generalKinFileNeeded) {
          close($macroFH);
          close($visFH);
          ## Mark general kin macros/files as already generated
          $generalKinFileNeeded=0;
        }
      }
    }
  }
}
close($gmnFH);

sub printError
{
  print "Could not open file $_[0]: $_[1]\n";
}
