#ifndef SLICER_INSTRUMENTS_H
#define SLICER_INSTRUMENTS_H

class Instrument{
	private:
		std::string name;
		float nozzle_radius_mm;
		float feed_rate;		//xy-movement across material
		float plunge_rate;	//z-movement across material
		float allowable_surface_tilt_angle_rad;
	public:
		Instrument(){};
		Instrument(std::string new_name, float new_radius, float new_feed_rate, float new_plunge_rate, float new_tilt){
			nozzle_radius_mm = new_radius;
			feed_rate = new_feed_rate;
			plunge_rate = new_plunge_rate;
			allowable_surface_tilt_angle_rad = new_tilt;
		}
		float GetRadius() { return nozzle_radius_mm; }
};

class InstrumentsSet {
	private:
		std::vector<Instrument> all_instruments;
		Instrument current_instrument;
	public:/*
		bool GetInstrumentsFromFile(std::string file_name){
			std::ifstream f_input(file_name);
			Instrument instr;
			while(!f_input.eof()) {
				//f_input.get(instr);
				all_instruments.push_back(instr);
			}
			f_input.close();
			if(all_instruments.empty())
				return false;
			current_instrument = all_instruments[0];
			return true;
		}
		bool ChooseByName(std::string name, Instrument *instr){
			for(auto it = all_instruments.begin(); it != all_instruments.end(); it++){
				if (name == it->name){
					*instr = *it;
					current_instrument = *it;
					return true;
				}
			}
			return false;
		}
		bool ChooseByNozzleDiameter(float diameter_mm, Instrument *instr){
			for(auto it = all_instruments.begin(); it != all_instruments.end(); it++){
				if (diameter_mm == it->nozzle_diameter_mm){
					*instr = *it;
					current_instrument = *it;
					return true;
				}
			}
			return false;	
		}
		Instrument GetCurrentInstrument() {return current_instrument;} */
};



#endif