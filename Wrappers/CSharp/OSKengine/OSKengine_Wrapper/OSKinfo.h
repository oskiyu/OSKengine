#pragma once

#include <OSKengine/OSKsettings.h>

namespace OSKengine {

	public ref class EngineInfo abstract sealed {

	public:

		static property System::String^ Name {
	public:
		System::String^ get() {
			return _Name;
		}
		}
		static property System::String^ VersionStage {
	public:
		System::String^ get() {
			return _VersionStage;
		}
		}
		static property System::String^ VersionMinor {
	public:
		System::String^ get() {
			return _VersionMinor;
		}
		}
		static property System::String^ VersionBuild {
	public:
		System::String^ get() {
			return _VersionBuild;
		}
		}
		static property System::String^ FullVersion {
	public:
		System::String^ get() {
			return _FullVersion;
		}
		}
		static property System::String^ VersionAlphaNumeric {
	public:
		System::String^ get() {
			return _VersionAlphaNumeric;
		}
		}
	
		static property System::Int32^ VersionStageNumeric {
	public:
		System::Int32^ get() {
			return _VersionStageNumeric;
		}
		}
		static property System::Int32^ VersVersionMinorNumericionMinor {
	public:
		System::Int32^ get() {
			return _VersionMinorNumeric;
		}
		}
		static property System::Int32^ VersionBuildNumeric {
	public:
		System::Int32^ get() {
			return _VersionBuildNumeric;
		}
		}
		
		static property System::String^ CS_WrapperBuild {
	public:
		System::String^ get() {
			return _CS_WrapperBuild;
		}
		}
		static property System::Int32^ CS_WrapperBuildNumeric {
	public:
		System::Int32^ get() {
			return _CS_WrapperBuildNumeric;
		}
		}

	private:

		static System::String^ _Name = gcnew System::String(OSK::ENGINE_NAME);
		static System::String^ _VersionStage = gcnew System::String(OSK::ENGINE_VERSION_STAGE);
		static System::String^ _VersionMinor = gcnew System::String(OSK::ENGINE_VERSION_MINOR);
		static System::String^ _VersionBuild = gcnew System::String(OSK::ENGINE_VERSION_BUILD);
		static System::String^ _FullVersion = gcnew System::String(OSK::ENGINE_VERSION);
		static System::String^ _VersionAlphaNumeric = gcnew System::String(OSK::ENGINE_VERSION_ALPHANUMERIC);

		static System::Int32 _VersionStageNumeric = OSK::ENGINE_VERSION_STAGE_NUMERIC;
		static System::Int32 _VersionMinorNumeric = OSK::ENGINE_VERSION_NUMERIC;
		static System::Int32 _VersionBuildNumeric = OSK::ENGINE_VERSION_BUILD_NUMERIC;

		static System::String^ _CS_WrapperBuild = "2020.11.19a";
		static System::Int32 _CS_WrapperBuildNumeric = 20201119;

	};

}