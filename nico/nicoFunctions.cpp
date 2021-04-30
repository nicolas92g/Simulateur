#include "nicoFunctions.h"

/**
 * .
 * @brief return the input string fully in UP char
 * \param string_
 * \return STRING_ 
 */
std::string nico::strings::entireStringToUpper(std::string string_) {
	std::string ret;
	for (uint32_t i = 0; i < string_.size(); i++)
	{
		ret += std::toupper(string_[i]);
	}
	return ret;
}
/**
 * @brief return the input string fully in low char.
 * 
 * \param STRING_
 * \return string_
 */
std::string nico::strings::entireStringToLower(std::string string_) {
	std::string ret;
	for (uint32_t i = 0; i < string_.size(); i++)
	{
		ret += std::tolower(string_[i]);
	}
	return ret;
}
/**
 * .
 * 
 * \param this allow to know if a char is in a string
 * \param chars  list of chars to test if one of them is  a 
 * \return 
 */
bool nico::strings::isOneOf(char a, std::string chars)
{
	for (unsigned int i = 0; i < chars.size(); i++)
	{
		if (a == char(chars[i])) { return true; }
	}
	return false;
}
/**
 * @brief read all the text from a file and put it in a string.
 * .
 * \param filepath file name to read
 * \return textInTheFile 
 */
std::string nico::strings::filepathToString(const char* filepath)
{
	std::string ret;
	std::ifstream file(filepath);
	std::string line;
	if (!file.is_open()) {
		return "";
	}
	while (getline(file, line)) {
		ret.append(line += '\n');
	}
	return ret;
}
/**
 * .
 * @brief fill a string var with the content of a file
 * \param &string string to fill
 * \param filepath file to read 
 * \return bool file can be open ?
 */
bool nico::strings::filepathToString(std::string& string, const char* filepath)
{
	std::ifstream file(filepath);
	std::string line;
	if (!file.is_open()) {
		return false;
	}
	string = "";
	while (getline(file, line)) {
		string.append(line += '\n');
	}
	return true;
}
/**
 * @brief transform a number in a string with a length given in param
 * .
 * \param length length of the output string 
 * \param number
 * \return string with a length of length
 */
template<typename numberType>
std::string nico::strings::intToString_lengthOf(int length, numberType number)
{
	std::string ret = std::to_string(number);
	if (ret.length() == length) {
		return ret;
	}
	if (ret.length() < length) {
		for (uint32_t i = 0; ret.length() < length; i++)
		{
			ret.insert(0, " ");
		}
	}
	else if (ret.length() > length) {
		ret.clear();
		for (uint32_t i = 0; i < length; i++)
		{
			ret += "9";
		}
	}
	return ret;
}

/**
 * @brief make a string of glm::vec4.
 * 
 * \param var
 * \return (x,y,z,w)
 */
std::string nico::strings::vec4Tostring(glm::vec4 var) {
	return  "(" + std::to_string(var.x) + ", " + std::to_string(var.y) + ", " + std::to_string(var.z) + ", " + std::to_string(var.w) + ")";
}

/**
 * @brief make a string of a glm::vec3.
 *
 * \param var
 * \return (x,y,z)
 */
std::string nico::strings::vec3Tostring(glm::vec3 var) {
	return  "(" + std::to_string(var.x) + ", " + std::to_string(var.y) + ", " + std::to_string(var.z) + ")";
}

/**
 * @brief make a string of a glm::vec2.
 *
 * \param var
 * \return (x,y)
 */
std::string nico::strings::vec2Tostring(glm::vec2 var) {
	return  "(" + std::to_string(var.x) + ", " + std::to_string(var.y) + ")";
}
/**
 * @brief make a string of glm::ivec4.
 *
 * \param var
 * \return (x,y,z,w)
 */
std::string nico::strings::ivec4Tostring(glm::ivec4 var) {
	return  "(" + std::to_string(var.x) + ", " + std::to_string(var.y) + ", " + std::to_string(var.z) + ", " + std::to_string(var.w) + ")";
}

/**
 * @brief make a string of a glm::ivec3.
 *
 * \param var
 * \return (x,y,z)
 */
std::string nico::strings::ivec3Tostring(glm::ivec3 var) {
	return  "(" + std::to_string(var.x) + ", " + std::to_string(var.y) + ", " + std::to_string(var.z) + ")";
}

/**
 * @brief make a string of a glm::vec2.
 *
 * \param var
 * \return (x,y)
 */
std::string nico::strings::ivec2Tostring(glm::ivec2 var) {
	return  "(" + std::to_string(var.x) + ", " + std::to_string(var.y) + ")";
}
/**
 * @brief check if a real is in [0,1].
 * 
 * \param nbr
 * \return
 */
bool nico::math::between0and1(float nbr)
{
	return (nbr >= 0 and nbr <= 1);
}
/**
 * @brief check if a real is in [0,1].
 *
 * \param nbr
 * \return
 */
bool nico::math::between0and1(double nbr)
{
	return (nbr >= 0 and nbr <= 1);
}