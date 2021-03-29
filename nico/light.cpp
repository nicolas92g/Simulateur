#include "light.h"

nico::BasicLight::BasicLight()
{
	setLightValues(glm::vec3(0.1f), glm::vec3(0.5), glm::vec3(0.5f));
}

void nico::BasicLight::setPos(glm::vec3 pos_)
{
	pos = pos_;
}

void nico::BasicLight::setLightValues(glm::vec3 ambient_, glm::vec3 diffuse_, glm::vec3 specular_)
{
	ambient = ambient_;
	color = diffuse_;
	specular = specular_;
}

void nico::BasicLight::setLight(glm::vec3 color, float ambIntensity, float diffIntensity, float specIntensity)
{
	ambient = color * ambIntensity;
	color = color * diffIntensity;
	specular = color * specIntensity;
}

void nico::BasicLight::setColor(glm::vec3 diff)
{
	color = diff;
}

void nico::BasicLight::setSpecular(glm::vec3 spec)
{
	specular = spec;
}

void nico::BasicLight::setAmbient(glm::vec3 amb)
{
	ambient = amb;
}

glm::vec3 nico::BasicLight::getPos()
{
	return pos;
}

uint32_t nico::Light::number = 0;
uint32_t nico::SpotLight::number = 0;

//---------------------
//attenuate light class  
//---------------------

nico::Light::Light(range range) : BasicLight()
{
	setRange(range);
}

nico::Light::Light(float linear, float quadratic, float constant) : BasicLight()
{
	setLightAttenuationValues(linear, quadratic, constant);
}

void nico::Light::setRange(range range)
{
	constant = 1.0f;
	switch (range)
	{
	case nico::Light::range::_7:
		linear = 0.7f;
		quadratic = 1.8f;
		break;
	case nico::Light::range::_13:
		linear = 0.35f;
		quadratic = 0.44f;
		break;
	case nico::Light::range::_20:
		linear = 0.22f;
		quadratic = 0.20f;
		break;
	case nico::Light::range::_32:
		linear = 0.14f;
		quadratic = 0.07f;
		break;
	case nico::Light::range::_50:
		linear = 0.09f;
		quadratic = 0.032f;
		break;
	case nico::Light::range::_65:
		linear = 0.07f;
		quadratic = 0.017f;
		break;
	case nico::Light::range::_100:
		linear = 0.045f;
		quadratic = 0.0075f;
		break;
	case nico::Light::range::_160:
		linear = 0.027f;
		quadratic = 0.0028f;
		break;
	case nico::Light::range::_200:
		linear = 0.022f;
		quadratic = 0.0019f;
		break;
	case nico::Light::range::_325:
		linear = 0.014f;
		quadratic = 0.0007f;
		break;
	case nico::Light::range::_600:
		linear = 0.007f;
		quadratic = 0.0002f;
		break;
	case nico::Light::range::_3250:
		linear = 0.0014f;
		quadratic = 0.000007f;
		break;
	case nico::Light::range::_inf:
		linear = 0.0f;
		quadratic = 0.0f;
		break;
	default:
		break;
	}
}

void nico::Light::setLightAttenuationValues(float linear, float quadratic, float constant)
{
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

void nico::Light::sendToShader(Shader* shader)
{
	shader->use();
	std::string obj = "light[" + std::to_string(Light::number) + "].";

	shader->set((obj + "position").c_str(), pos);
	shader->set((obj + "ambient").c_str(), ambient);
	shader->set((obj + "diffuse").c_str(), color);
	shader->set((obj + "specular").c_str(), specular);

	shader->set((obj + "constant").c_str(), constant);
	shader->set((obj + "linear").c_str(), linear);
	shader->set((obj + "quadratic").c_str(), quadratic);
	
	Light::number++;
	shader->set("numberOfLight", Light::number);
}

void nico::Light::clear()
{
	Light::number = 0;
}


#ifdef NICO_RENDERING_DEBUG
void nico::Light::importFromJson(std::string jsonString)
{

	Json::Value root;
	Json::String err;

	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

	bool worked = reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.length(), &root, &err);

	if (worked) {
		for (auto it = root.begin(); it != root.end(); it++) {
			std::cout << it.name() << " : " << *it << std::endl;
			if (it.name() == this->name) {
				

				for (Json::Value::iterator it2 = it->begin(); it2 != it->end(); it2++) {
					if (it2.name() == "color") {
						this->setColor(glm::make_vec3(Editor::getVecFromJson(*it2)));
						nico::println(this->color);
					}
				}
			}
		}

		//std::cout << "\n\nstyledString: \n" << root.toStyledString();
	}
	else { std::cout << "failed to load " + this->name + " with Json : " << err; }
}
#endif // NICO_RENDERING_DEBUG

//---------------
//class SpotLight
//---------------

nico::SpotLight::SpotLight(glm::vec3 direction, float innerAngle, float outerAngle) : Light(){
	setSpotValues(direction, innerAngle, outerAngle);
}

void nico::SpotLight::setSpotValues(glm::vec3 direction, float innerAngle, float outerAngle)
{
	//fill direction if it is (0,0,0)
	if (direction == glm::vec3(0.0f))
		direction = glm::vec3(1, 0, 0);

	this->direction = glm::normalize(direction);
	this->innerAngle = this->checkAngle(innerAngle);
	this->outerAngle = this->checkAngle(outerAngle);
}

void nico::SpotLight::setDirection(glm::vec3 direction)
{
	if (direction != glm::vec3(0.0f))
		this->direction = glm::normalize(direction);
}

void nico::SpotLight::setSpotAngle(float innerAngle, float outerAngle)
{
	this->innerAngle = checkAngle(innerAngle);
	this->outerAngle = checkAngle(outerAngle);
}

void nico::SpotLight::setSpotAngle(float angle, int transitionPercentageOfAngle) {
	this->innerAngle = checkAngle(angle);
	this->outerAngle = checkAngle(this->innerAngle + this->innerAngle * ((float)transitionPercentageOfAngle / 100.0f));
}

void nico::SpotLight::sendToShader(Shader* shader)
{
	shader->use();
	std::string obj = "spotLight[" + std::to_string(SpotLight::number) + "].";

	shader->set((obj + "position").c_str(), pos);
	shader->set((obj + "ambient").c_str(), ambient);
	shader->set((obj + "diffuse").c_str(), color);
	shader->set((obj + "specular").c_str(), specular);

	shader->set((obj + "constant").c_str(), constant);
	shader->set((obj + "linear").c_str(), linear);
	shader->set((obj + "quadratic").c_str(), quadratic);
				 
	shader->set((obj + "direction").c_str(), direction);
	shader->set((obj + "cutOff").c_str(), glm::cos(glm::radians(innerAngle)));
	shader->set((obj + "outerCutOff").c_str(), glm::cos(glm::radians(outerAngle)));

	number++;
	shader->set("numberOfSpot", SpotLight::number);
}	

void nico::SpotLight::clear()
{
	SpotLight::number = 0;
}

float nico::SpotLight::checkAngle(float angle)
{
	if (angle < 0.1f) {
		return 0.1f;
	}
	else if (angle > 360.0f){
		return angle = 360.0f;
	}
	else {
		return angle;
	}
}

//----------------------
//class directionalLight
//----------------------

nico::DirectionalLight::DirectionalLight(glm::vec3 direction)
{
	this->direction = direction;
}

void nico::DirectionalLight::setDirection(glm::vec3 direction)
{
	if (direction != glm::vec3(0.0f))
		this->direction = glm::normalize(direction);
}

glm::vec3 nico::DirectionalLight::getDirection() const
{
	return this->direction;
}

void nico::DirectionalLight::sendToShader(Shader* shader)
{
	shader->use();
	shader->set( "dirLight.direction", direction);
	shader->set( "dirLight.ambient", ambient);
	shader->set( "dirLight.diffuse", color);
	shader->set( "dirLight.specular", specular);
}

void nico::DirectionalLight::importFromJson(std::string jsonString)
{
}

void nico::clearLights()
{
	Light::clear();
	SpotLight::clear();
}

