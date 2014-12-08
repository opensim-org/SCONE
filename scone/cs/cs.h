#pragma once

#ifdef CS_EXPORTS
#define CS_API __declspec(dllexport)
#else
#define CS_API __declspec(dllimport)
#endif

// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )

namespace scone
{
	namespace cs
	{
		class Measure;
		typedef std::unique_ptr< Measure > MeasureUP;
		typedef std::shared_ptr< Measure > MeasureSP;

		class ParameterizableController;
		typedef std::shared_ptr< ParameterizableController > ParameterizableControllerSP;
	}
}
