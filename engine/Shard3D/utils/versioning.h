#pragma once
#include <string>
namespace Shard3D {
	enum class VersionState {
		PreAlpha, Alpha, Beta, Stable, Experimental
	};
	struct Version {
		Version() = default;
		Version(VersionState _state, int _major, int _minor, int _patch = 0, int _revision = 0) :
			major(_major), minor(_minor), patch(_patch), revision(_revision), state(_state) {}
		Version(Version& other) = delete;
		Version operator=(Version& other) = delete;

		bool potentiallyUnstable() const { return state != VersionState::Stable; }
		std::string toString() const {
			switch (state) {
			case VersionState::PreAlpha:
				return std::string(
					std::to_string(major) + "." +
					std::to_string(minor) + "." +
					std::to_string(patch) + ".r" +
					std::to_string(revision) +
					"-prealpha"
				);
				break;
			case VersionState::Alpha:
				return std::string(
					std::to_string(major) + "." +
					std::to_string(minor) + "." +
					std::to_string(patch) + ".r" +
					std::to_string(revision) +
					"-alpha"
				);
				break;
			case VersionState::Beta:
				return std::string(
					std::to_string(major) + "." +
					std::to_string(minor) + "." +
					std::to_string(patch) + ".r" +
					std::to_string(revision) +
					"-beta"
				);
				break;
			case VersionState::Stable:
				return std::string(
					std::to_string(major) + "." +
					std::to_string(minor) + "." +
					std::to_string(patch) + ".r" +
					std::to_string(revision)
				);
				break;
			case VersionState::Experimental:
				return std::string(
					std::to_string(major) + "." +
					std::to_string(minor) + "." +
					std::to_string(patch) + ".r" +
					std::to_string(revision) +
					"-experimental"
				);
				break;
			}
		}
		bool operator <(Version& other) { return major < other.major && minor < other.minor && patch < other.patch && revision < other.revision; }
		bool operator >(Version& other) { return major > other.major && minor > other.minor && patch > other.patch && revision > other.revision; }
		bool operator ==(Version& other) { return major == other.major && minor == other.minor && patch == other.patch && revision == other.revision; }
		bool operator !=(Version& other) { return !(*this == other); }

		operator int() const { return major & minor & patch & revision; }
		inline operator std::string () const {  
			return toString();
		}
	private:
		unsigned int major, minor, patch, revision;
		VersionState state = VersionState::PreAlpha;
	};
}