#include <memory>

#ifdef SIM_EXPORTS
#define SIM_API __declspec(dllexport)
#else
#define SIM_API __declspec(dllimport)
#endif

namespace scone
{
	namespace sim
	{
		typedef std::shared_ptr< class Model > ModelSharedPtr;
		typedef std::shared_ptr< class Body > BodySharedPtr;
		typedef std::shared_ptr< class Controller > ControllerSP;
	}
}
