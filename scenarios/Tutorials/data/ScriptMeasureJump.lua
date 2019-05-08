function init( model )
	-- get the 'target_body' parameter from ScriptMeasure, or set to "pelvis"
	target_body = scone.target_body or "pelvis"

	-- find the actual body with the same name
	body = model:find_body( target_body )

	-- global boolean that keeps track if we have moved upwards
	has_moved_up = false
end

function update( model )
	-- get current vertical position and velocity
	local pos = body:com_pos().y
	local vel = body:com_vel().y

	-- check if we are at peak height
	if has_moved_up then -- have we been moving up?
		if vel < 0 then -- are we moving down?
			-- we are at the peak!
			return true -- return true to end the simulation
		end
	else
		if vel > 0.01 then -- are we starting to move up?
			has_moved_up = true -- set the flag
		end
	end

	return false -- return false to keep going
end

function result( model )
	-- return the vertical position as the final result
	-- this is either peak height
	-- or the height at the end of the simulation
	return body:com_pos().y
end
