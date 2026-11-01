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
    double energy = 0; // UD
    double mass = 0; // UD
    // vertex [cm]
    double vx;
    double vy;
    double vz;
};


#endif