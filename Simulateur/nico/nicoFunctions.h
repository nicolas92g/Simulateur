/*****************************************************************//**
 * \file   nicoFunctions.h
 * \brief  some useful functions
 * 
 * \author nicolas
 * \date   November 2020
 *********************************************************************/

#pragma once
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <deque>
#include <vector>
#include <glad/glad.h>
#include <config.hpp>


//remove or put namespace into the buid
#define STRINGS
#define MATH

/**
 *.
 * @namespace nico
 * @brief contains all the classes and a few functions
 */
namespace nico
{

#ifdef STRINGS
	/**
	 * .
	 * namespace nico::strings
	 * @brief contains some functions usefull with std::string and char 
	 */
	namespace strings {
		/**
		 * @brief put a full string in uppercase
		 */
		std::string entireStringToUpper(std::string string_);
		/**
		 * @brief put a full string in lowercase
		 */
		std::string entireStringToLower(std::string string_);
		/**
		 * @brief find a char in a string 
		 */
		bool isOneOf(char a, std::string chars);
		/**
		 * @brief put all the text from a file path in a string
		 */
		std::string filepathToString(const char * filepath);
		/**
		 * @brief put all the text from a file path in a string and return if the filepath was finded
		 */
		bool filepathToString(std::string& string, const char* filepath);
		template<typename numberType>
		/**
		 * @brief convert a number into a string of a given size
		 */
		std::string intToString_lengthOf(int length, numberType number);
		/**
		 * @brief convert a vector in a string
		 */
		std::string vec4Tostring(glm::vec4);
		/**
		 * @brief convert a vector in a string
		 */
		std::string vec3Tostring(glm::vec3);
		/**
		 * @brief convert a vector in a string
		 */
		std::string vec2Tostring(glm::vec2);
	}
#endif //strings

#ifdef MATH
	/**
	 * .
	 * namespace nico::math
	 * @brief contain some math useful functions 
	 */
	namespace math {
		/**
		 * @brief check if a number is between 0 included and 1 included
		 */
		bool between0and1(float nbr);
		/**
		 * @brief check if a number is between 0 included and 1 included
		 */
		bool between0and1(double nbr);
		template <typename T>
		std::vector<T> organizeAscending(std::vector<T> listOfValue);
		template <typename T>
		T greater(std::vector<T> listOfValue);
		template <typename T>
		T smaller(std::vector<T> listOfValue);
		template <typename T>
		T meanOf(std::vector<T> listOfvalue);
		template <typename T>
		T meanOf(std::deque<T> listOfvalue);

	}
#endif // math

	template<typename T>
	/**
	 * @brief this a print function compatible with glm::vec types
	 */
	void print(T value);
	template<typename T>
	/**
	 * @brief this a print function compatible with glm::vec types 
	 * and add a \n at the end of the print
	 */
	void print(T value, int numberOfValuesPerLines);

	template<typename T>
	void println(T value);
	template<typename T>
	/**
	 * @brief usefull in a function to return a var and then change the value of the var
	 */
	T returnThenAssign(T* value, T newValue);
	
}

//TEMPLATES :

/**
 * .
 * @brief organize a array in ascending order
 * \param listOfValue a std::vector<T> with T a type equipped of operator>() and operator=() 
 * \return std::vector<T> which is organized in ascending order
 */
template <typename T>
std::vector<T> nico::math::organizeAscending(std::vector<T> listOfValue) {
	for (uint32_t i = listOfValue.size(); i > 0; i--)
	{
		for (uint32_t j = 0; j < i - 1; j++) {
			if (listOfValue[j] > listOfValue[j + 1]) {
				T var = listOfValue[j];
				listOfValue[j] = listOfValue[j + 1];
				listOfValue[j + 1] = var;
			}
		}
	}
	return listOfValue;
}
/**
 * .
 * @brief return the greater element of a list
 * \param list of T which is equiped of operator<(), operator=() and operator[]
 * \return T greater element
 */
template<typename T>
T nico::math::greater(std::vector<T> list)
{
	if (!list.size()) {
		return T(NULL);
	}
	return nico::math::organizeAscending<T>(list)[list.size() - 1];
}
/**
 * .
 * @brief return smaller element of an array of T 
 * \param listOfValue
 * \return T smaller
 */
template<typename T>
T nico::math::smaller(std::vector<T> listOfValue)
{
	if (!listOfValue.size()) {
		return T(NULL);
	}
	return nico::math::organizeAscending<T>(listOfValue).operator[](0);
}
/**
 * @brief return the mean of a list of T which is equipped of operator[], operator+(), operator/()
 * .
 * \param listOfvalue 
 * \return T mean
 */
template<typename T>
T nico::math::meanOf(std::vector<T> listOfvalue)
{
	uint64_t ret = 0;
	for (uint64_t i = 0; i < listOfvalue.size(); i++)
	{
		ret += listOfvalue[i];
	}
	ret /= listOfvalue.size();
	return ret;
}
/**
 * @brief return the mean of a list of T which is equipped of operator[], operator+(), operator/()
 * .
 * \param listOfvalue
 * \return T mean
 */
template<typename T>
T nico::math::meanOf(std::deque<T> listOfvalue)
{
	T ret = 0;
	for (uint32_t i = 0; i < listOfvalue.size(); i++)
	{
		ret += listOfvalue[i];
	}
	ret /= listOfvalue.size();
	return ret;
}

/**
 * @brief compatible with glm::vecX type and display them like this : (x, y, z)
 * \param value
 */
template<typename T>
void nico::print(T value)
{
	std::cout << value;
}
template<>
inline void nico::print(glm::vec2 value)
{
	std::cout << "("
		+ std::to_string(value.x) + ", \t"
		+ std::to_string(value.y) + ")";
}
template<>
inline void nico::print(glm::vec3 value)
{
	std::cout << "("
		+ std::to_string(value.x) + ", \t"
		+ std::to_string(value.y) + ", \t"
		+ std::to_string(value.z) + ")";
}
template<>
inline void nico::print(glm::vec4 value)
{
	std::cout << "("
		+ std::to_string(value.x) + ", \t"
		+ std::to_string(value.y) + ", \t"
		+ std::to_string(value.z) + ", \t"
		+ std::to_string(value.w) + ")";
}

/**
 * .
 * 
 * \param value
 * \param numberOfValuesPerLines
 */
template<typename T>
void nico::print(T value, int numberOfValuesPerLines)
{
	std::cout << "{\n";
	for (size_t i = 0; i < value.size(); i++)
	{
		std::cout << "\t" + std::to_string(value[i]) + ", ";
		if (!((i+1) % numberOfValuesPerLines)) {
			std::cout << "\n" << (i - 1)/3 + 1 << " :";
		}
	}
	std::cout << "}\n";
}

template<typename T>
void nico::println(T value)
{
	print<T>(value);
	printf("\n");
}

template<typename T>
T nico::returnThenAssign(T* value, T newValue)
{
	T ret= *value;
	*value = newValue;
	return ret;
}



