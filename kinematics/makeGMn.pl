#!/usr/bin/perl

## For easier debuging, require strict perl rules
use strict;

## Get the configuration we are running
my $num_args = $#ARGV + 1;
if($num_args != 1) {
  print "Must specify configuration\n";
  exit -1;
}

my $config = $ARGV[0];
print "Parsing config $config\n";

## Standard GMm configurations
my $numOfEvents = 10000000;
my $cmuQueue = "green";
my $magPathLength =  1.2192; ## Given in m (from dimensions in G4SBS)
my $intFieldMap = 1.73824;  ## Integrated field of Bogdan's field map at x=0,y=0
#my $magPathLength =  1.2192+0.0002; ## Given in m (from dimensions in G4SBS)
## The next line is deduced from Robin's max field strength to field
## setting (from her e-mail):
##    "GMn will not be configured to use the maximum current. The highest
##    integral in the GMn configurations is 1.71E+06 Gauss-cm and Field of
##    1.0528 Tesla."
#my $magPathLength =  1.6242; ## Given in m

my $kinNum = 0;
my $maxGenRuns = 10;  ## Generate macros/scripts for N runs per kin setting
#my @voffsetList = (0.0, 15.0, 30.0, 45.0);
#my @kinSetList = ("april","may");
my @voffsetList = (0.0);
my $macPath = "scripts/${config}";
my $rootPath = "rootfiles/${config}";
my $fieldMapPath = "fields/${config}";
my $cmuBatchPath = "cmu_batch/${config}";
my $jlabBatchPath = "jlab_batch/${config}";
my $rootGeneratedPath = "rootfiles/generated";
my $plotBatchPath = "$jlabBatchPath/plot";

## Make the paths
mkdir $macPath;
mkdir $rootPath;
mkdir $cmuBatchPath;
mkdir $jlabBatchPath;
mkdir $rootGeneratedPath;
mkdir $plotBatchPath;
mkdir $fieldMapPath;

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

my $shrinkMacro = "";
my $copyGenFile = "";

open(my $gmnFH, "<kinematics/gmn_${config}.txt")
  or die printError("kinematics/gmn_${config}.txt",$!);
while(my $line = <$gmnFH>) {
  ## Strip end of line character
  chomp($line);
  ## Only accept lines that do not begin with a #
  if( !($line =~ /^\#/) ) {
    # Found a valid line, let's process it
    my $kin = sprintf("%02d",++$kinNum);

    my $macroNamePrefix = "$macPath/gmn_kin";
    my $csvNamePrefix = "$macPath/comment_gmn_kin";
    my $visNamePrefix = "$macPath/vis_gmn_kin";
    my $runMacroNamePrefix = "$macPath/run_gmn_kin";
    my $fieldMapNamePrefix = "$fieldMapPath/gmn_kin";
    my $rootKinPath = "$rootPath/kin${kin}";
    mkdir $rootKinPath;
    my $rootNamePrefix = "${rootKinPath}/gmn_kin";
    my $rootGeneratedKinPath = "$rootGeneratedPath/kin${kin}";
    mkdir $rootGeneratedKinPath;
    my $rootGeneratedNamePrefix = "$rootGeneratedKinPath/generated_gmn_kin";
    my $visRootNamePrefix = "$rootPath/vis_gmn_kin";
    my $cmuBatchNamePrefix = "$cmuBatchPath/cmu_gmn_kin";
    my $jlabBatchNamePrefix = "$jlabBatchPath/jlab_gmn_kin";
    my $jlabBatchLogNamePrefix = "$jlabBatchPath/logs/jlab_gmn_kin";
    my $plotBatchNamePrefix = "$plotBatchPath/plot_gmn_kin";
    my $plotBatchLogNamePrefix = "$plotBatchPath/logs/plot_gmn_kin";


    my ($tmp,$q2,$beam_e,$theta_bb,$theta_sbs,$dist_bb,$dist_mag,$dist_hcal,$bdl,
      #$th_min,$th_max,$ph_min,$ph_max) = split(" ",$line);
      $gen,$golden) = split(" ",$line);
    my $th_min=$theta_bb-10;
    my $th_max=$theta_bb+10;
    my $ph_min=-30;
    my $ph_max= 30;

    if( ($config eq "generated") && ($gen ne "yes")) {
      next;
    }

    ## Determine magnetic field using the standard 48 inch length of 48D48
    #my $field_mag = $bdl/$magPathLength; ## Given in Tesla
    ## Round off to 4 decimal places
    #$field_mag = sprintf("%.4f",$field_mag);
    # ## Scale the TOSCA map file for GEp for use in GMn kinematics.
    # ## (Andrew thinks it is about 2.33 Tm, based on a 5degree bend radius)
    # my $magScale = $bdl/2.33;
    # $magScale = sprintf("%.4f",$magScale);  ## Round off to 4 decimal places
    # my $fieldMapDist = -61-$dist_mag*100;
    # $fieldMapDist = sprintf("%.1f",$fieldMapDist);

    ## New field map from Bogdan.  Positioned at -1.8 m instead (so needs
    ## to be moved back to the appropriate distance)
    my $fieldMapDist = 180-$dist_mag*100;
    $fieldMapDist = sprintf("%.1f",$fieldMapDist);
    my $fieldMapTheta = 180+$theta_sbs;
    ## Also, seems like the field is about 1.05 T, need to scale our field
    #my $magScale = $field_mag/1.05;
    #$magScale = sprintf("%.4f",$magScale);  ## Round off to 4 decimal places
    ## Scale the magnetic field to match requested integrated field
    ## for each kinematic.
    ## Start with the value I get when I integrate Bogdan's field map
    ## for all Z at X=Y=0;
    my $magScale = $bdl/$intFieldMap;

    print "Generating g4sbs files for Kin$kin.\n";
    foreach my $voffset (@voffsetList) {
      ## Flag that specifies we need to build the general kin files
      ## (only needed once per kinematic and voffset setting)
      my $generalKinFileNeeded = 1;
      for(my $run = 0; $run < $maxGenRuns; $run++ ) {
        my $runString = sprintf("%03d",$run);
        #my $runFileComment = "$kin\_r$runString\_v$voffset";
        #my $runFileSmallComment = "$kin\_r$runString\_v$voffset\_small";
        #my $kinFileComment = "$kin\_v$voffset";

        my $runFileComment = "$kin\_r$runString";
        my $runFileSmallComment = "$kin\_r$runString\_small";
        my $kinFileComment = "$kin";

        ## Define some standard file names
        my $runMacroFileName = "$runMacroNamePrefix$runFileComment.mac";
        my $cmuBatchFileName = "$cmuBatchNamePrefix$runFileComment.sh";
        my $jlabBatchFileName = "$jlabBatchNamePrefix$runFileComment.xml";
        my $jlabBatchLogFilePrefix = "$jlabBatchLogNamePrefix$runFileComment";
        my $plotBatchLogFilePrefix = "$plotBatchLogNamePrefix$kinFileComment";
        my $rootFileName = "$rootNamePrefix$runFileComment.root";
        my $rootFileNameSmall = "$rootNamePrefix$runFileSmallComment.root";
        my $rootGeneratedFileName = "$rootGeneratedNamePrefix$kin\_r$runString.root";
        my $rootGeneratedFileNameSmall = "$rootGeneratedNamePrefix$kin\_r$runString\_small.root";
        my $macroFileName = "$macroNamePrefix$kinFileComment.mac";
        my $csvFileName = "$csvNamePrefix$kinFileComment.csv";
        my $visFileName = "$visNamePrefix$kinFileComment.mac";
        my $visRootFileName = "$visRootNamePrefix$kinFileComment.root";
        my $jlabSaveRootFileXMLCmnd = "";
        my $plotBatchFileName = "$plotBatchNamePrefix$kinFileComment.xml";
        my $fieldMapFileName = "$fieldMapNamePrefix$kinFileComment\_from_Bodan_20160806.header";
        my $fieldMapFullFileName = "$fieldMapNamePrefix$kinFileComment\_from_Bodan_20160806.map";
        my $genCmd = "";
        $copyGenFile = 0;

        if( $config eq "generated" ) {
          $rootFileName = $rootGeneratedFileName;
          $rootFileNameSmall = $rootGeneratedFileNameSmall;
          $shrinkMacro = " shrinkGenerated.C";
        } else {
          $shrinkMacro = " shrinkGMn.C";
        }

        if($gen eq "yes") {
          if( $config eq 'generated' ) {
            $genCmd = "/g4sbs/generateOnly 1";
            $copyGenFile = 0;
          } else {
            $genCmd = "/g4sbs/filegeneratorfile $rootGeneratedFileName";
            $copyGenFile = 1;
          }
        }


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
        my $plotBatchFH;
        my $fieldMapFH;
        my $csvFH;
        if($generalKinFileNeeded) {
          open($macroFH, ">",$macroFileName)
            or die printError($macroFileName,$!);
          open($visFH, ">", $visFileName)
            or die printError($visFileName,$!);
          open($plotBatchFH, ">", $plotBatchFileName)
            or die printError($plotBatchFileName,$!);
          open($csvFH, ">", $csvFileName)
            or die printError($csvFileName,$!);
          open($fieldMapFH, ">", $fieldMapFileName)
            or die printError($fieldMapFileName,$!);
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

          ## Now produce the plotAcceptance Batch file
          open(my $inPlotBatchTemplate, "<", "kinematics/template_gmn_plot_batch.xml")
            or die printError("kinematics/template_gmn_plot_batch.xml");
          while(<$inPlotBatchTemplate>) {
            s/%%(\$\w+)%%/$1/eeg;
            print $plotBatchFH $_;
          }
          close($inPlotBatchTemplate);

          ## Now produce the plotAcceptance Batch file
          open(my $inCSVTemplate, "<", "kinematics/template_gmn.csv")
            or die printError("kinematics/template_gmn.csv");
          while(<$inCSVTemplate>) {
            s/%%(\$\w+)%%/$1/eeg;
            print $csvFH $_;
          }
          close($inCSVTemplate);



          ## Add vis menu to GUI
          print $guiFH "/gui/addButton kins Kin$kin ";
          print $guiFH "\"/control/execute $visFileName\"\n";

          ## Generate the fieldMap header
          print $fieldMapFH "0.0 0.0 $fieldMapDist\n";
          print $fieldMapFH "0. $fieldMapTheta 0.0\n";

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
          close($fieldMapFH);
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
