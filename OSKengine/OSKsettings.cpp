#include "OSKsettings.h"

namespace OSK {

#ifdef AUTODEF_BUILD
	std::string FromAsciiToDecimal(int ascii) {
		return std::to_string(ascii - 48);
	}

	std::string GetDay(const std::string& date) {
		std::string output = "";

		if (date[6] == ' ') {
			output += "0";
			output += FromAsciiToDecimal(date[5]);
		}
		else {
			output += FromAsciiToDecimal(date[5]);
			output += FromAsciiToDecimal(date[6]);
		}

		return output;
	}
	
	std::string GetMonth(const std::string& date) {
		std::string subString = date.substr(0, 3);

		if (subString == "Jan")
			return "01";
		if (subString == "Feb")
			return "02";
		if (subString == "Mar")
			return "03";
		if (subString == "Apr")
			return "04";
		if (subString == "May")
			return "05";
		if (subString == "Jun")
			return "06";
		if (subString == "Jul")
			return "07";
		if (subString == "Aug")
			return "08";
		if (subString == "Sep")
			return "09";
		if (subString == "Oct")
			return "10";
		if (subString == "Nov")
			return "11";
		if (subString == "Dec")
			return "12";

		return "99";
	}

	const std::string DATE = __DATE__;
	const int length = DATE.length() - 1;

	std::string YEAR = FromAsciiToDecimal(DATE[length - 3]) + DATE[length - 2] + DATE[length - 1] + DATE[length];
	std::string MONTH = GetMonth(DATE);
	std::string DAY = GetDay(DATE);
#endif
	std::string YEAR = "2021";
	std::string MONTH = "05";
	std::string DAY = "27";

	std::string ENGINE_NAME = "OSKengine";

	std::string ENGINE_VERSION_STAGE = "Alpha";

	int ENGINE_VERSION_STAGE_NUMERIC = 0;

	int ENGINE_VERSION_NUMERIC = 17;

	std::string ENGINE_VERSION_MINOR = std::to_string(ENGINE_VERSION_NUMERIC);

	std::string ENGINE_VERSION_ALPHANUMERIC = ENGINE_VERSION_STAGE + " " + ENGINE_VERSION_MINOR;

	std::string ENGINE_VERSION_BUILD = YEAR + "." + MONTH + "." +DAY + "a";
	int ENGINE_VERSION_BUILD_NUMERIC = 9;
	std::string ENGINE_VERSION = "0.17+ build " + ENGINE_VERSION_BUILD;

	int ENGINE_VERSION_TOTAL = 17;

}