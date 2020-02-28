function init( model )
    -- This function is called at the start of the simulation
    -- 'model' can be used to initialize the measure parameters (see LuaModel)
end

function update( model )
    -- This function is called at each simulation timestep
    -- Use it to update the internal variables of the measure (if needed)
    return false -- change to 'return true' to terminate the simulation early
end

function result( model )
    -- This function is called at the end of the simulation
    -- It should return the result of the measure
    return 0.0
end

function store_data( current_frame )
    -- This function is called at each simulation timestep
    -- 'current_frame' can be used to store values for analysis (see LuaFrame)
end
