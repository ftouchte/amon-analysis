/**********************************************
 * Lund file generator
 *
 * cf. https://gemc.jlab.org/gemc/html/documentation/generator/lund.html
 *
 * @author : ftouchte
 * @date : November 25, 2025
 * @revisited : November 01, 2026
 * *******************************************/

#ifndef GENERATE_LUND_FILE_H
#define GENERATE_LUND_FILE_H

#include <cmath>

struct LundHeader {
    //***  number of particle to generate in this event
    int nbOfParticles;
    // mass number of the target (UD)
    int A;
    // atomic number (UD)
    int Z;
    // target polarisation (UD)
    double polarization = 0; 
    //*** first particle Z component of spin
    double spin = 0; 
    // 11 for electron and 22 for photon (UD)
    int beam_type = 11;
    // beam energy (GeV) (UD)
    double beam_energy; 
    // interacted nucleon ID (2212 or 2112) (UD)
    int nucleon_id = 2212;
    // process ID (UD)
    int process_id = 1;
    // event weight (UD)
    double event_weight = 1;

};

struct LundParticle {
    int index;
    double lifetime = -1; // UD
    int type = 1;
    int ID;
    int parent_index = 0;
    int daughter_index = 0; // UD
    // momentum [GeV]
    double px;
    double py;
    double pz;
    double p;
    double theta; // radians
    double phi; // radians
    double energy = 0; // UD
    double mass = 0; // UD
    // vertex [cm]
    double vx;
    double vy;
    double vz;
};

void polar2cart(double p, double theta, double phi, double & px, double & py, double & pz) {
    pz = p*cos(theta);
    px = p*sin(theta)*cos(phi);
    py = p*sin(theta)*sin(phi);
}

double uniform(double a, double b, int nevents) {
    double x = (1.0*(rand() % nevents))/nevents;
    return (b-a)*x + a;
}

void generate_kinematics(int nevents, LundParticle & electron, LundParticle & track) {
    // beam energy
    double Ee = 2.23951; // GeV
    double nu = 0.06966; // GeV, nu = E - E', this the mean of the distribution
    // electron
    double phi = uniform(0,360, nevents)*M_PI/180; // deg to rad
    double theta = uniform(7,8, nevents)*M_PI/180;
    double p = Ee - nu; // GeV
    
    polar2cart(p, theta, phi, electron.px, electron.py, electron.pz);
    electron.p = p;
    electron.theta = theta;
    electron.phi = phi;
    // track
    double track_p = 2*Ee*sin(theta/2);
    double track_pz = Ee*(1-cos(theta));
    double track_theta = acos(track_pz/track_p);
    double track_phi = (phi > M_PI) ? (phi - M_PI) : (phi + M_PI);
    polar2cart(track_p, track_theta, track_phi, track.px, track.py, track.pz);
    track.p = track_p;
    track.theta = track_theta;
    track.phi = track_phi;
    // vertex
    double vertex = uniform(-15,15, nevents); // cm
    electron.vx = 0;
    electron.vy = 0;
    electron.vz = vertex;
    track.vx = 0;
    track.vy = 0;
    track.vz = vertex;
}


#endif