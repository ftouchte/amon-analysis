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
    TH1D* H1_electron_p = new TH1D("electron_p", "p; p (GeV); count", 100, 2.17, 2.20);
    TH1D* H1_electron_theta = new TH1D("electron_theta", "theta; theta (deg); count", 100, 0, 90);
    TH1D* H1_electron_phi = new TH1D("electron_phi", "phi; phi (deg); count", 100, 0, 361);
    TH1D* H1_electron_vz = new TH1D("electron_vz", "vz; vz (cm); count", 100, -15, 15);
    TH1D* H1_track_p = new TH1D("track_p", "p; p (GeV); count", 100, 0.15, 0.5);
    TH1D* H1_track_theta = new TH1D("track_theta", "theta; theta (deg); count", 100, 0, 90);
    TH1D* H1_track_phi = new TH1D("track_phi", "phi; phi (deg); count", 100, 0, 361);
    TH1D* H1_track_vz = new TH1D("track_vz", "vz; vz (cm); count", 100, -15, 15);

    try {
        // Header
        LundHeader header;
        header.nbOfParticles = config["N"].get<int>();
        header.A = config["A"].get<int>();
        header.Z = config["Z"].get<int>();
        header.polarization = config["polarization"].get<double>(); 
        header.spin = config["spin"].get<double>(); 
        header.beam_type = config["beam_type"].get<int>();
        header.beam_energy = config["beam_energy"].get<double>(); 
        header.nucleon_id = config["interacted_nucleon"].get<int>();
        header.process_id = config["process_id"].get<int>();
        header.event_weight = config["event_weight"].get<double>();

        // electron
        LundParticle electron;
        electron.index = config["electron"]["index"].get<int>();
        electron.lifetime = config["electron"]["lifetime"].get<double>();
        electron.type = config["electron"]["type"].get<int>();
        electron.ID = config["electron"]["PID"].get<int>();
        electron.parent_index = config["electron"]["parent_index"].get<int>();
        electron.daughter_index = config["electron"]["daughter_index"].get<int>();
        electron.px = 0;
        electron.py = 0;
        electron.pz = 0;
        electron.mass = config["electron"]["mass"].get<double>();
        electron.energy = 0;
        electron.vx = 0;
        electron.vy = 0;
        electron.vz = 0;

        // track
        LundParticle track;
        track.index = config["target"]["index"].get<int>();
        track.lifetime = config["target"]["lifetime"].get<double>();
        track.type = config["target"]["type"].get<int>();
        track.ID = config["target"]["PID"].get<int>();
        track.parent_index = config["target"]["parent_index"].get<int>();
        track.daughter_index = config["target"]["daughter_index"].get<int>();
        track.px = 0;
        track.py = 0;
        track.pz = 0;
        track.mass = config["target"]["mass"].get<double>();
        track.energy = 0;
        track.vx = 0;
        track.vy = 0;
        track.vz = 0;

        // Generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> gen_delta_phi(
            config["generator"]["delta_phi"]["mean"].get<double>(), 
            config["generator"]["delta_phi"]["sigma"].get<double>());
        std::normal_distribution<double> gen_W2(
            config["generator"]["W2"]["mean"].get<double>(), 
            config["generator"]["W2"]["sigma"].get<double>());
        std::normal_distribution<double> gen_electron_theta(
            config["generator"]["electron_theta"]["mean"].get<double>(), 
            config["generator"]["electron_theta"]["sigma"].get<double>());
        std::uniform_real_distribution<double> gen_electron_phi(0, 360);
        std::uniform_real_distribution<double> gen_vertex(
            config["generator"]["vertex"]["min"].get<double>(), 
            config["generator"]["vertex"]["max"].get<double>());

        int nevents = 10000;

        for (int i = 0; i <= nevents; i++) {

            // pT is the same for the electron and for the AHDC track
            // theta and phi for the electron are free
            double W2 = gen_W2(gen);
            double phi = gen_electron_phi(gen)*Units::deg;
            double theta = gen_electron_theta(gen)*Units::deg;
            double M = track.mass;
            double E = header.beam_energy;
            double p = (M*M + 2*M*E - W2)/(4*E*pow(sin(theta/2),2.0)+2*M);
            double vz = gen_vertex(gen);
            electron.px = p*sin(theta)*cos(phi);
            electron.py = p*sin(theta)*sin(phi);
            electron.pz = p*cos(theta);
            electron.vz = vz;
            track.px = -p*sin(theta)*cos(phi);
            track.py = -p*sin(theta)*sin(phi);
            track.pz = E-p*cos(theta);
            track.vz = vz;

            // Update kinematics
            fill_histograms(electron, H1_electron_p, H1_track_theta, H1_electron_phi);
            H1_electron_vz->Fill(vz);
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
    f->Close();    
    
    // close file
    ofs.close();
    std::cout << "File created : " << lund_file << std::endl; 
    std::cout << "File created : " << TString::Format("%s.root", lund_file.c_str()).Data() << std::endl; 

    return 0;

}
