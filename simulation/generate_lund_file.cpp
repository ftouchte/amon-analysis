/**********************************************
 * Lund file generator for the elastic
 * events : e N -> e N
 *
 * cf. https://gemc.jlab.org/gemc/html/documentation/generator/lund.html
 *
 * @author : ftouchte
 * @date : November 25, 2025
 * @revisited : November 01, 2026
 * *******************************************/

 #include "generate_lund_file.h"

#include <fstream>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <random>
#include <filesystem>
#include <iostream>

#include "TH1.h"
#include "TFile.h"
#include "TString.h"
#include "fOptions.h"



int main(int argc, const char * argv[]) {

    /// --- Load files from options
    fOptions OPT({"-json", "-out_lund"});
    OPT.LoadOptions(argc, argv);
    OPT.Show();

    std::string json_file = OPT.GetValue("-json");
    std::string lund_file = OPT.GetValue("-out_lund");

    if (json_file == "" || lund_file == "") {
        std::cout << "Please, provide all the options." << std::endl;
    }

    // Create stream
    std::ofstream ofs(lund_file.c_str());
    
    // Header
    LundHeader header;
    header.nbOfParticles = 2;
    header.A = 2;
    header.Z = 1;
    header.polarization = 0; 
    header.spin = 0; 
    header.beam_type = 11;
    header.beam_energy = 2.24; 
    header.nucleon_id = 2212;
    header.process_id = 1;
    header.event_weight = 1;
    // electron
    LundParticle electron;
    electron.index = 1;
    electron.lifetime = -1;
    electron.type = 1;
    electron.ID = 11;
    electron.parent_index = 0;
    electron.daughter_index = 0;
    electron.px = 0.24;
    electron.py = 0;
    electron.pz = 2.17;
    electron.mass = 0.511e-3;
    electron.energy = sqrt(pow(electron.mass,2) + pow(electron.px,2) + pow(electron.py,2) + pow(electron.pz,2)); // UD
    electron.vx = 0;
    electron.vy = 0;
    electron.vz = 0;
    // track
    LundParticle track;
    track.index = 2;
    track.lifetime = -1;
    track.type = 1;
    track.ID = 1000010020; //   this is a deuteron
    track.parent_index = 0;
    track.daughter_index = 0;
    track.px = -0.24;
    track.py = 0;
    track.pz = 0.03;
    track.mass = 1.875;
    track.energy = sqrt(pow(track.mass,2) + pow(track.px,2) + pow(track.py,2) + pow(track.pz,2)); // UD
    track.vx = 0;
    track.vy = 0;
    track.vz = 0;
    
    // Histograms
    TH1D* H1_electron_p = new TH1D("electron_p", "p; p (GeV); count", 100, 2.17, 2.20);
    TH1D* H1_electron_theta = new TH1D("electron_theta", "theta; theta (deg); count", 100, 0, 90);
    TH1D* H1_electron_phi = new TH1D("electron_phi", "phi; phi (deg); count", 100, 0, 361);
    TH1D* H1_electron_vz = new TH1D("electron_vz", "vz; vz (cm); count", 100, -15, 15);
    TH1D* H1_track_p = new TH1D("track_p", "p; p (GeV); count", 100, 0.15, 0.5);
    TH1D* H1_track_theta = new TH1D("track_theta", "theta; theta (deg); count", 100, 0, 90);
    TH1D* H1_track_phi = new TH1D("track_phi", "phi; phi (deg); count", 100, 0, 361);
    TH1D* H1_track_vz = new TH1D("track_vz", "vz; vz (cm); count", 100, -15, 15);

    int nevents = 10000;

    for (int i = 0; i <= nevents; i++) {
        // Update kinematics
        generate_kinematics(nevents, electron, track);
        H1_electron_p->Fill(electron.p);
        H1_electron_theta->Fill(electron.theta*180/M_PI);
        H1_electron_phi->Fill(electron.phi*180/M_PI);
        H1_electron_vz->Fill(electron.vz);
        H1_track_p->Fill(track.p);
        H1_track_theta->Fill(track.theta*180/M_PI);
        H1_track_phi->Fill(track.phi*180/M_PI);
        H1_track_vz->Fill(track.vz);
        // Ici l'ordre est très important !!!
        // Print Header
        ofs << header.nbOfParticles << " ";
        ofs << header.A << " ";
        ofs << header.Z << " ";
        ofs << header.polarization << " "; 
        ofs << header.spin << " "; 
        ofs << header.beam_type << " ";
        ofs << header.beam_energy << " "; 
        ofs << header.nucleon_id << " ";
        ofs << header.process_id << " ";
        ofs << header.event_weight << std::endl;
        // Print electron
        ofs << electron.index << " ";
        ofs << electron.lifetime << " ";
        ofs << electron.type << " ";
        ofs << std::setw(10) << electron.ID << " ";
        ofs << electron.parent_index << " ";
        ofs << electron.daughter_index << " ";
        ofs << electron.px << " ";
        ofs << electron.py << " ";
        ofs << electron.pz << " ";
        ofs << electron.energy << " ";
        ofs << electron.mass << " ";
        ofs << electron.vx << " ";
        ofs << electron.vy << " ";
        ofs << electron.vz << std::endl;
        // Print track
        ofs << track.index << " ";
        ofs << track.lifetime << " ";
        ofs << track.type << " ";
        ofs << std::setw(10) << track.ID << " ";
        ofs << track.parent_index << " ";
        ofs << track.daughter_index << " ";
        ofs << track.px << " ";
        ofs << track.py << " ";
        ofs << track.pz << " ";
        ofs << track.energy << " ";
        ofs << track.mass << " ";
        ofs << track.vx << " ";
        ofs << track.vy << " ";
        ofs << track.vz << std::endl;
    }
    // save in root file
    TFile *f = new TFile("./output/lund_file.root", "RECREATE");
    H1_electron_p->Write("electron_p");
    H1_electron_theta->Write("electron_theta");
    H1_electron_phi->Write("electron_phi");
    H1_electron_vz->Write("electron_vz");
    H1_track_p->Write("track_p");
    H1_track_theta->Write("track_theta");
    H1_track_phi->Write("track_phi");
    H1_track_vz->Write("track_vz");
    f->Close();
    printf("File created : %s\n", "./output/lund_file.root");
    
    
    // close file
    ofs.close();
    std::cout << "File created : " << lund_file << std::endl; 

    return 0;

}
