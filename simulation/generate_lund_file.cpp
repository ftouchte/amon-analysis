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
#include "Units.h"

#include <nlohmann/json.hpp>
 
using json = nlohmann::json;


void fill_histograms(const LundParticle & particle, TH1D* h_p, TH1D* h_theta, TH1D* h_phi) {
    double px = particle.px;
    double py = particle.py;
    double pz = particle.pz;
    double p = sqrt(px*px + py*py + pz*pz);
    double theta = acos(pz/p);
    double phi = atan2(py,px);
    if (phi < 0) phi = phi + 2*M_PI;
    h_p->Fill(p);
    h_theta->Fill(theta/Units::deg);
    h_phi->Fill(phi/Units::deg);
}

void progressBar(int state, int bar_length = 100) { // state is a number between 0 and 100
    // for the moment the bar length is not variable
    if (state > bar_length) {return ;}
    printf("\rProgress \033[32m\[");
    for (int i = 0; i <= state; i++) {
        printf("#");
    }
    printf("\033[0m");
    for (int i = state+1; i < bar_length; i++) {
        printf(".");
    }
    if (state == 100) {
        printf("\033[32m] \033[1m %d %%\033[0m\n", state);
    } else {
        printf("] %d %%", state);
    }
    fflush(stdout);
}

int main(int argc, const char * argv[]) {

    /// --- Load files from options
    fOptions OPT({"-json", "-o"});
    OPT.LoadOptions(argc, argv);
    OPT.Show();

    std::string json_file = OPT.GetValue("-json");
    std::string lund_file = OPT.GetValue("-o");

    if (json_file == "" || lund_file == "") {
        std::cout << "Please, provide all the options." << std::endl;
        return 1;
    }

    // Read JSON config
    std::ifstream ifs(json_file.c_str());
    if (!ifs.is_open()) {
        std::cerr << "Cannot open " << json_file << std::endl;
        return 1;
    }

    json config;
    try {
        // 3e argument : allow_exceptions, 4e argument : ignore_comments
        config = json::parse(ifs, nullptr, true, true);
    } catch (const json::parse_error& e) {
        std::cerr << "Error parsing : " << e.what() << '\n';
        return 1;
    }

    // Create stream
    std::ofstream ofs(lund_file.c_str());

    // Histograms
    TH1D* H1_electron_p = new TH1D("electron_p", "p; p (GeV); count", 100, 2.1, 2.3);
    TH1D* H1_electron_theta = new TH1D("electron_theta", "theta; theta (deg); count", 100, 3, 10);
    TH1D* H1_electron_phi = new TH1D("electron_phi", "phi; phi (deg); count", 100, -5, 365);
    TH1D* H1_electron_vz = new TH1D("electron_vz", "vz; vz (cm); count", 100, -16, 16);
    TH1D* H1_track_p = new TH1D("track_p", "p; p (GeV); count", 100, 0.15, 0.35);
    TH1D* H1_track_theta = new TH1D("track_theta", "theta; theta (deg); count", 100, 50, 100);
    TH1D* H1_track_phi = new TH1D("track_phi", "phi; phi (deg); count", 100, -5, 365);
    TH1D* H1_track_vz = new TH1D("track_vz", "vz; vz (cm); count", 100, -16, 16);
    TH1D* H1_W2 = new TH1D("W2", "W2; W2 (Gev^{2}); count", 50, 3.2, 4);

    int nevents = 1000;

    try {
        // Header
        LundHeader header;
        header.nbOfParticles = config.at("header").at("N").get<int>();
        header.A = config.at("header").at("A").get<int>();
        header.Z = config.at("header").at("Z").get<int>();
        header.polarization = config.at("header").at("polarization").get<double>(); 
        header.spin = config.at("header").at("spin").get<double>(); 
        header.beam_type = config.at("header").at("beam_type").get<int>();
        header.beam_energy = config.at("header").at("beam_energy").get<double>(); 
        header.nucleon_id = config.at("header").at("interacted_nucleon").get<int>();
        header.process_id = config.at("header").at("process_id").get<int>();
        header.event_weight = config.at("header").at("event_weight").get<double>();

        // electron
        LundParticle electron;
        electron.index = config.at("electron").at("index").get<int>();
        electron.lifetime = config.at("electron").at("lifetime").get<double>();
        electron.type = config.at("electron").at("type").get<int>();
        electron.ID = config.at("electron").at("PID").get<int>();
        electron.parent_index = config.at("electron").at("parent_index").get<int>();
        electron.daughter_index = config.at("electron").at("daughter_index").get<int>();
        electron.px = 0;
        electron.py = 0;
        electron.pz = 0;
        electron.mass = config.at("electron").at("mass").get<double>();
        electron.energy = 0;
        electron.vx = 0;
        electron.vy = 0;
        electron.vz = 0;

        // track
        LundParticle track;
        track.index = config.at("target").at("index").get<int>();
        track.lifetime = config.at("target").at("lifetime").get<double>();
        track.type = config.at("target").at("type").get<int>();
        track.ID = config.at("target").at("PID").get<int>();
        track.parent_index = config.at("target").at("parent_index").get<int>();
        track.daughter_index = config.at("target").at("daughter_index").get<int>();
        track.px = 0;
        track.py = 0;
        track.pz = 0;
        track.mass = config.at("target").at("mass").get<double>();
        track.energy = 0;
        track.vx = 0;
        track.vy = 0;
        track.vz = 0;

        // Generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> gen_electron_theta(
            config.at("generator").at("electron_theta").at("min").get<double>(), 
            config.at("generator").at("electron_theta").at("max").get<double>());
        std::uniform_real_distribution<double> gen_electron_phi(
            config.at("generator").at("electron_phi").at("min").get<double>(), 
            config.at("generator").at("electron_phi").at("max").get<double>());
        std::uniform_real_distribution<double> gen_vertex(
            config.at("generator").at("vertex").at("min").get<double>(), 
            config.at("generator").at("vertex").at("max").get<double>());

        for (int i = 1; i <= nevents; i++) {

            progressBar(100.0*i/nevents);

            //double pT = gen_electron_pT(gen)*Units::deg;
            double phi = gen_electron_phi(gen)*Units::deg;
            double theta = gen_electron_theta(gen)*Units::deg;
            double vz = gen_vertex(gen);
            double M = track.mass;
            double E = header.beam_energy;
            // W^2 should match M^2, p and theta are correlated
            double p = (2*M*E)/(4*E*pow(sin(theta/2),2.0)+2*M);
            electron.px = p*sin(theta)*cos(phi);
            electron.py = p*sin(theta)*sin(phi);
            electron.pz = p*cos(theta);
            electron.vz = vz;
            track.px = -p*sin(theta)*cos(phi);
            track.py = -p*sin(theta)*sin(phi);
            track.pz = E-p*cos(theta);
            track.vz = vz;

            // W2
            double nu = E - p;
            double Q2 = 4*E*p*pow(sin((theta/2)*M_PI/180),2);
            double W2 = pow(track.mass,2) + 2*track.mass*nu - Q2;
            H1_W2->Fill(W2);


            // Update kinematics
            fill_histograms(electron, H1_electron_p, H1_electron_theta, H1_electron_phi);
            H1_electron_vz->Fill(electron.vz);
            fill_histograms(track, H1_track_p, H1_track_theta, H1_track_phi);
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
    } catch (const json::exception& e) {
        std::cerr << "Erreur de configuration : " << e.what() << '\n';
        return 1;
    }
    
    
    // save in root file
    TFile *f = new TFile(TString::Format("%s.root", lund_file.c_str()).Data(), "RECREATE");
    H1_electron_p->Write("electron_p");
    H1_electron_theta->Write("electron_theta");
    H1_electron_phi->Write("electron_phi");
    H1_electron_vz->Write("electron_vz");
    H1_track_p->Write("track_p");
    H1_track_theta->Write("track_theta");
    H1_track_phi->Write("track_phi");
    H1_track_vz->Write("track_vz");
    H1_W2->Write("W2");
    f->Close();    
    
    // close file
    ofs.close();
    std::cout << "File created : " << lund_file << std::endl; 
    std::cout << "File created : " << TString::Format("%s.root", lund_file.c_str()).Data() << std::endl; 

    return 0;

}
