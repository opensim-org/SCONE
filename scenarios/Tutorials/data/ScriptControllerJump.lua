-- SCONE script for a simple feed-forward controller.
-- See Tutorial 6a - Script - High Jump

function init( model, par )
	-- keep a list of offsets and slopes to compute the excitation
	offset = {}
	slope = {}

	-- keep a list of all actuators
	actuators = {}

	-- iterate over all actuators in the model
	for i = 1, model:actuator_count() do
		-- store the actuator in the list
		actuators[ i ] = model:actuator( i )

		-- create parameters for both slope and offset
		local name = actuators[ i ]:name()
		offset[ i ] = par:create_from_mean_std( name .. "-offset", 0.3, 0.1, 0, 1 )
		slope[ i ] = par:create_from_mean_std( name .. "-slope", 0, 1, -10, 10 )
	end
end

function update( model )
	-- get the current simulation time
	local t = model:time()

	-- iterate over all actuators
	for i = 1, #actuators do
		local excitation = offset[ i ] + t * slope[ i ]
		actuators[ i ]:add_input( excitation )
	end
end
