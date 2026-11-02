# Simulation

I will come back to this later.

A lot of things changed in simulation.

I try to make the generation of LUND file more natural.

To deal with JSON files, I use this framework

- **nlohmann/json** : https://github.com/nlohmann/json

Example of use:

```bash
 ./generate_lund_file -json ../lund_config.json -o ./output/deuteron.dat
 ```
