//#pragma once
//#include <nico/Object2d.h>
//#include <nico/NumberInput.h>
//
//namespace nico{
//	//generic case
//	template<class C, typename T> class Input2d : public Object2d
//	{
//	public:
//		Input2d(Window* win, void( C::* setFunction)(T));
//	protected:
//		void(C::*func)(T);
//	};
//
//	//double 
//	template<class C> class Input2d<C, double> : public NumberInput
//	{
//	public:
//		Input2d(Window* win, void(C::* setFunction)(double));
//		void draw(Shader* shader);
//
//	protected:
//		void(C::* func)(double);
//	};
//	
//	//bool 
//	template<class C> class Input2d<C, bool> : public Button
//	{
//	public:
//		Input2d(Window* win, void(C::* setFunction)(bool));
//		void draw(Shader* shader);
//
//	protected:
//		void(C::* func)(double);
//	};
//	//std::string 
//	template<class C> class Input2d<C, std::string> : public TextInput
//	{
//	public:
//		Input2d(Window* win, void(C::* setFunction)(std::string));
//		void draw(Shader* shader);
//
//	protected:
//		void(C::* func)(double);
//	};
//	//glm::vec<glm::length_t L, float, glm::packed_highp>
//	template<class C> class Input2d<C, glm::vec<3, float, glm::packed_highp>> : public TextInput
//	{
//	public:
//		Input2d(Window* win, void(C::* setFunction)(std::string));
//		void draw(Shader* shader);
//
//	protected:
//		void(C::* func)(double);
//	};
//	
//	//undefined case
//	template<class C, typename T>
//	inline Input2d<C, T>::Input2d(Window* win, void(C::* setFunction)(T))
//	{
//		static_assert("this type is not supported by nico::Input2d");
//	}
//
//	//double
//	template<class C>
//	inline Input2d<C, double>::Input2d(Window* win, void(C::* setFunction)(double)) : NumberInput(win)
//	{
//		func = setFunction;
//	}
//	template<class C>
//	inline void nico::Input2d<C, double>::draw(Shader* shader)
//	{
//		if (this->selected) {
//			this->func(this->getValue());
//		}
//		NumberInput::draw(shader);
//	}
//	//bool
//	template<class C>
//	inline Input2d<C, bool>::Input2d(Window* win, void(C::* setFunction)(bool)) : NumberInput(win)
//	{
//		func = setFunction;
//	}
//	template<class C>
//	inline void nico::Input2d<C, bool>::draw(Shader* shader)
//	{
//		if (this->selected) {
//			this->func(this->isClicked());
//		}
//		NumberInput::draw(shader);
//	}
//	//std::string
//	template<class C>
//	inline Input2d<C, std::string>::Input2d(Window* win, void(C::* setFunction)(std::string)) : NumberInput(win)
//	{
//		func = setFunction;
//	}
//	template<class C>
//	inline void nico::Input2d<C, std::string>::draw(Shader* shader)
//	{
//		if (this->selected) {
//			this->func(this->text);
//		}
//		NumberInput::draw(shader);
//	}
//}
