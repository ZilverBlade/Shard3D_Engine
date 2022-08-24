#include "../../s3dpch.h"  
#include "hudmgr.h"
#include <fstream>
#include "../asset/assetmgr.h"

namespace Shard3D {
	namespace ECS {
		HUDManager::HUDManager(HUDContainer* container) {
			mHud = container;
			SHARD3D_INFO("Loading HUD Manager");
		}

		std::string HUDManager::encrypt(std::string input) {
			auto newTime = std::chrono::high_resolution_clock::now();
			char c;
			std::string encryptedString;
			for (int i = 0; i < input.length(); i++) {
				c = input.at(i);
				encryptedString.push_back((char)
					((((c + WB3D_CIPHER_KEY) * 2) - WB3D_CIPHER_KEY) / 2));
			}

			SHARD3D_LOG("Duration of HUD encryption: {0} ms", std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - newTime).count() * 1000);
			return encryptedString;
		}
		std::string HUDManager::decrypt(std::string input) {
			auto newTime = std::chrono::high_resolution_clock::now();
			char c;
			std::string decryptedString;
			for (int i = 0; i < input.length(); i++) {
				c = input.at(i);
				decryptedString.push_back((char)
					(((c * 2) + WB3D_CIPHER_KEY) / 2) - WB3D_CIPHER_KEY);
			}

			SHARD3D_LOG("Duration of HUD decryption: {0} ms", std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - newTime).count() * 1000);
			return decryptedString;
		}

		static void saveElement(YAML::Emitter& out, sPtr<HUDElement> element) {
			if (element->guid == 0) return; // might be reserved for core engine purposes

			out << YAML::BeginMap;
			out << YAML::Key << "Element" << YAML::Value << element->guid;
			{
				out << YAML::Key << "Info";
				out << YAML::BeginMap;
					out << YAML::Key << "Tag" << YAML::Value << element->tag;
				out << YAML::EndMap;
			} 
			{
				out << YAML::Key << "Transform2D";
				out << YAML::BeginMap;
					out << YAML::Key << "Position" << YAML::Value << element->position;
					out << YAML::Key << "Scale" << YAML::Value << element->scale;
					out << YAML::Key << "Rotation" << YAML::Value << element->rotation;
					out << YAML::Key << "Zpos" << YAML::Value << element->zPos;
					out << YAML::Key << "Anchor" << YAML::Value << element->anchorOffset;
				out << YAML::EndMap;
			}		
			{
				out << YAML::Key << "Texture";
				out << YAML::BeginMap;
					out << YAML::Key << "DefaultTex" << YAML::Value << element->default_texture;
					out << YAML::Key << "HoverTex" << YAML::Value << element->hover_texture;
					out << YAML::Key << "PressTex" << YAML::Value << element->press_texture;
				out << YAML::EndMap;
			}		
			{
				out << YAML::Key << "Script";
				out << YAML::BeginMap;
				out << YAML::Key << "Module" << YAML::Value << element->scriptmodule;
				out << YAML::Key << "Language" << YAML::Value << (element->scriptlang ? "C#" : "VB");
				out << YAML::Key << "Static" << !element->isActive;
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}
		static void saveLayer(YAML::Emitter& out, HUDContainer* hudLayerInfo, int layer) {
			out << YAML::Key << "Elements" << YAML::Value << YAML::BeginSeq;
			for (auto& element : hudLayerInfo->getList().at(layer)->elements) {
				saveElement(out, element.second);
			}
			out << YAML::EndSeq;
		}
		void HUDManager::save(const std::string& destinationPath, int layer, bool encryptLevel) {
			YAML::Emitter out;
			out << YAML::BeginMap;
			out << YAML::Key << "Shard3D" << YAML::Value << ENGINE_VERSION.toString();
			out << YAML::Key << "SHARD3D" << YAML::Value << EDITOR_VERSION.toString();
			out << YAML::Key << "GUI_Template" << YAML::Value << "Some kind of template";
			out << YAML::Key << "Layer" << YAML::Value << layer;
			saveLayer(out, mHud, layer);
			out << YAML::EndMap;
			std::string newPath = destinationPath;
			if (!strUtils::hasEnding(destinationPath, ".wbht")) newPath = destinationPath + ".wbht";
			if (encryptLevel) {
				std::ofstream fout(newPath);
				fout << encrypt(out.c_str());
				fout.flush();
				fout.close();
			} else {
				std::ofstream fout(newPath);
				fout << out.c_str();
				fout.flush();
				fout.close();
			}
		
			SHARD3D_LOG("Saved scene '{0}'", newPath);
		}

		void HUDManager::saveRuntime(const std::string& destinationPath) {
			SHARD3D_ASSERT(false, "");
		}

		HUDMgrResults HUDManager::load(const std::string& sourcePath, int layer, bool ignoreWarns) {
			std::ifstream stream(sourcePath);
			std::stringstream strStream;
			strStream << stream.rdbuf();

#ifdef _DEPLOY
			//YAML::Node data = YAML::Load(decrypt(strStream.str()));
			YAML::Node data = YAML::Load(strStream.str());
#if ENSET_BETA_DEBUG_TOOLS
			std::ofstream fout(sourcePath + ".dcr");
			fout << decrypt(strStream.str());
			fout.flush();
			fout.close();
#endif
#else
			YAML::Node data = YAML::Load(strStream.str());
#endif

			if (ignoreWarns == false) {
				if (!data["GUI_Template"]) return HUDMgrResults::WrongFileResult;

				if (data["Shard3D"].as<std::string>() != ENGINE_VERSION.toString()) {
					SHARD3D_WARN("Incorrect engine version");
					return HUDMgrResults::OldEngineVersionResult;
				}// change this to check if the version is less or more
				if (data["SHARD3D"].as<std::string>() != EDITOR_VERSION.toString()) {
					SHARD3D_WARN("Incorrect editor version");
					return HUDMgrResults::OldEditorVersionResult;
				}// change this to check if the version is less or more

			}
			
			if (data["Elements"]) {
				SHARD3D_LOG("Loading layer {0}", layer);
				HUD obj;
				for (auto element : data["Elements"]) {
					if (mHud->getList().at(layer)->elements.find(element["Element"].as<uint64_t>()) != mHud->getList().at(layer)->elements.end()) {
						SHARD3D_WARN("Trying to load HUD element with guid {0} when it already is present! Element will be ignored...", element["Element"].as<uint64_t>());
						continue;
					}
					
					sPtr<HUDElement> loadedElement = make_sPtr<HUDElement>();
					UUID guid = element["Element"].as<uint64_t>();
					SHARD3D_LOG("Loading HUD element {0}", guid);
					obj.elements.emplace(guid, loadedElement);
					loadedElement->guid = guid;
					loadedElement->tag = element["Info"]["Tag"].as<std::string>();

					loadedElement->position = element["Transform2D"]["Position"].as<glm::vec2>();
					loadedElement->scale = element["Transform2D"]["Scale"].as<glm::vec2>();
					loadedElement->rotation = element["Transform2D"]["Rotation"].as<float>();
					loadedElement->zPos = element["Transform2D"]["Zpos"].as<int>();
					loadedElement->anchorOffset = element["Transform2D"]["Anchor"].as<glm::vec2>();

					loadedElement->default_texture = element["Texture"]["DefaultTex"].as<std::string>();
					ResourceHandler::loadTexture(loadedElement->default_texture);
					loadedElement->hover_texture = element["Texture"]["HoverTex"].as<std::string>();
					ResourceHandler::loadTexture(loadedElement->hover_texture);
					loadedElement->press_texture = element["Texture"]["PressTex"].as<std::string>();
					ResourceHandler::loadTexture(loadedElement->press_texture);
					loadedElement->scriptmodule = element["Script"]["Module"].as<std::string>();
					loadedElement->scriptlang = element["Script"]["Language"].as<std::string>() == "C#" ? 0 : 1;
					loadedElement->isActive = !element["Script"]["Static"].as<bool>();
				}
				mHud->swap(layer, &obj);
			}
			return HUDMgrResults::SuccessResult;
		}

		HUDMgrResults HUDManager::loadRuntime(const std::string& sourcePath) {
			SHARD3D_ASSERT(false, "");
			return HUDMgrResults::ErrorResult;
		}
	}
}