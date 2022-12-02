/*
 * GPSFix.h
 *
 *  Created on: Jul 23, 2014
 *      Author: Cameron Karlsson
 *
 *  See the license file included with this source.
 */

#ifndef GPSFIX_H_
#define GPSFIX_H_

#include <cstdint>
#include <ctime>
#include <string>
#include <chrono>
#include <vector>
#include <cmath>
#include <sstream>

namespace nmea {

	class GPSSatellite;
	class GPSAlmanac;
	class GPSFix;
	class GPSService;


	// =========================== GPS SATELLITE =====================================

	class GPSSatellite {
	public:
		GPSSatellite() :
			snr(0),
			prn(0),
			elevation(0),
			azimuth(0)
		{};

		//satellite data
		double snr;			// 0-99 dB
		uint32_t prn;		// id - 0-32
		double elevation;	// 0-90 deg
		double azimuth;		// 0-359 deg
		std::string toString();
		operator std::string();
	};






	// =========================== GPS ALMANAC =====================================


	class GPSAlmanac {
		friend GPSService;
	private:
		uint32_t visibleSize;
		uint32_t lastPage;
		uint32_t totalPages;
		uint32_t processedPages;
		void clear();			//will remove all information from the satellites
		void updateSatellite(GPSSatellite sat);
	public:
		GPSAlmanac() :
			lastPage(0),
			totalPages(0),
			processedPages(0)
		{};

		//mapped by prn
		std::vector<GPSSatellite> satellites;
		double averageSNR();
		double minSNR();
		double maxSNR();
		double percentComplete();

	};




	// =========================== GPS TIMESTAMP =====================================

	// UTC time
	class GPSTimestamp {
	private:
		std::string monthName(uint32_t index);
	public:
		GPSTimestamp();

		int32_t hour;
		int32_t min;
		double sec;

		int32_t month;
		int32_t day;
		int32_t year;

		// Values collected directly from the GPS
		double rawTime;
		int32_t rawDate;

		time_t getTime();

		// Set directly from the NMEA time stamp
		// hhmmss.sss
		void setTime(double raw_ts);

		// Set directly from the NMEA date stamp
		// ddmmyy
		void setDate(int32_t raw_date);

		std::string toString();
	};


	// =========================== Attitude data =====================================
	class GPSAttitude {

	public:
		std::string toString();

		GPSTimestamp timestamp;
		double heading{0.};			// degrees true north (0-360)
		double roll{0.};			// degrees
		double pitch{0.};			// degrees
		double headingDeviation{0.};// degrees
		double rollDeviation{0.};	// degrees
		double pitchDeviation{0.};	// degrees
		int32_t sattelitesCount{0};	// Number of satellites used for attitude computation
		int8_t modeIndicator{0};	// Mode indicator:
					 					// 0: No attitude available
					 					// 1: Heading, Pitch with float ambiguities
					 					// 2: Heading, Pitch with fixed ambiguities
					 					// 3: Heading, Pitch, Roll with float ambiguities
					 					// 4: Heading, Pitch, Roll with fixed ambiguities
					 					// 5: Heading, Pitch from velocity (dead-reckoning)
					 					// 6: Heading, Pitch, Roll from non-RTK INS
					 					// 7: Heading, Pitch, Roll from RTK INS
					 					// 8: Heading, Pitch, Roll from INS coasting
		double magneticVariation{0.};// degree
		char magnetVarDirection{'E'};// E=East, W=West
	};



	// =========================== GPS FIX =====================================

	class GPSFix {
		friend GPSService;
		
	private:

		bool haslock{false};
		bool setlock(bool b);		//returns true if lock status **changed***, false otherwise.

	public:
		GPSAlmanac almanac;
		GPSTimestamp timestamp;

		char status{'V'};		// Status: A=active, V=void (not locked)
		uint8_t type{1};		// Type: 1=none, 2=2d, 3=3d
		uint8_t quality{0};	// Quality: 
							//    0 = invalid
							//    1 = GPS fix (SPS)
							//    2 = DGPS fix
							//    3 = PPS fix
							//    4 = Real Time Kinematic (RTK)
							//    5 = Float RTK
							//    6 = estimated (dead reckoning) (2.3 feature)

		double dilution{0.};					// Combination of Vertical & Horizontal
		double horizontalDilution{0.};			// Horizontal dilution of precision, initialized to 100, best =1, worst = >20
		double verticalDilution{0.};			// Vertical is less accurate

		double altitude{0.};		// meters
		double latitude{0.};		// degrees N
		double longitude{0.};		// degrees E
		double speed{0.};			// km/h
		double travelAngle{0.};		// degrees true north (0-360)
		GPSAttitude attitude;
		int32_t trackingSatellites{0};
		int32_t visibleSatellites{0};

		bool locked();
		double horizontalAccuracy();
		double verticalAccuracy();
		bool hasEstimate();

		std::chrono::seconds timeSinceLastUpdate();	// Returns seconds difference from last timestamp and right now.

		std::string toString();
		operator std::string();

		static std::string travelAngleToCompassDirection(double deg, bool abbrev = false);
	};

}

#endif /* GPSFIX_H_ */
