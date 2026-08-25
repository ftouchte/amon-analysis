# Time reconstruction of saturated signals

All the details of the works are given in this presentation ([link](https://clasweb.jlab.org/wiki/images/a/a9/Ftc_time_reconstruction_saturated_signals.pdf)).

This repository contians the source code.

## Usage

```bash
# Create a folder
mkdir build && cd build

# Create Makefile
cmake ..

# Create executable : run and run_all_hits are created
make

# Run only hit from track
run -i hipo/rec_10files_21476_after_wf_requalification.hipo -o ./root/output_after_wf_requalification.root

run -i hipo/rec_10files_21476_pull_request.hipo -o ./root/output_pull_request.root

# Run all hits for more statistics
run_all_hits -i hipo/rec_10files_21476_after_wf_requalification.hipo -o ./root/output_after_wf_requalification_all_hits.root

```

## Details
1. `run` analyses hits belonging to existing tracks. All the standard plots are there.

1. `run_all_hits` analyses all hits, belonging or not to a tracks. The motivation was to get more statistics. Here, particularly, the comparison between the ToT method and the slope method is done.

1. Tips: after the option -i, one can provide a list of files. E.g: `./run -i file_[123].hipo -o test.root`

## Warning

1. The analysis has been made post p0v11 of RGL, so coatjava version 14.1.2. I particularly recooked the 10 files of run 21476 from tghe available p0v11.

1. For the analysis, one can use any files. It just requires that the following banks exist in the file: AHDC::wf, AHDC::adc, AHDC::hits, AHDC::track, REC::Particle

## Files to reproduce the results of the presentation

```
cd /w/hallb-scshelf2102/clas12/users/touchte/analysis-files/saturated-signals/

tree
.
├── hipo
│   ├── rec_10files_21476_after_wf_requalification.hipo
│   ├── rec_10files_21476_coat_dev0.hipo
│   └── rec_10files_21476_pull_request.hipo
└── root
    ├── output_after_wf_requalification.root
    ├── output_after_wf_requalification_all_hits.root
    └── output_pull_request.root


```
