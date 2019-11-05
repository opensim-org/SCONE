function init( model, par )
	for i = 1, model:body_count() do
		b = model:body( i )
		p = b:com_pos()
		p2 = 0.1 * p + vec3:new( 1, 0, 0 )
		p3 = -p / 100
		p3:normalize()
		unitx = vec3:new( 1, 0, 0 )
		cross = vec3.cross_product( p, unitx )
		dot = p:dot_product( unitx )
		p2:clear()
		l = p:length()
		sl = p:squared_length()
		scone.info( b:name() .. ": " ..
			tostring(p) ..
			" cross=" .. tostring(cross) ..
			" dot=" .. tostring(dot) ..
			" p3=" .. tostring(p3)
			)
	end
end

function update( model )
	-- get the current simulation time
	local t = model:time()
end
