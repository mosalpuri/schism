
#pragma optimize(on)
#pragma debug(off)

#extension ARB_texture_rectangle : require
#extension ARB_texture_rectangle : enable

#version 120

// interpolants
varying vec3            _cam_pos;
varying vec3            _ray_dir;
varying vec3            _obj_pos;
varying vec3            _view_dir;

// uniforms
uniform vec2            _screen_dimensions;

uniform vec3            _voxel_size;

uniform sampler3D       _volume;
uniform sampler1D       _color_alpha;

uniform sampler2DRect   _geom_depth;

uniform float           _step_size;

#ifdef RAYCAST_INSIDE_VOLUME
uniform vec4            _near_plane;
#endif

// global consts
const vec3  lumi_convert = vec3(0.3, 0.59, 0.11);
const float alpha_termination = float(0.99);

vec3 debug_col;

// function declarations
vec4    color_lookup(in vec4 inp);
vec4    do_shading(in vec3 sample_pos, in vec4 inp);
bool    inside_volume_bounds(const in vec3  position,
                             const in float geom_sample_dist);
vec3    calculate_starting_point(const in vec3 ray_step);
float   calculate_fragment_depth_objspc();

// implementation
vec4 color_lookup(in vec4 inp)
{
    return (texture1D(_color_alpha, inp.r));
}

vec4 do_shading(in vec3 sample_pos, in vec4 inp)
{
    vec4 ret;

    ret = color_lookup(inp);
    //ret.rgb = pow(ret.rgb, vec3(10.0));
    //ret.a = 1.0 - pow(1.0 - ret.a, 10.0);
#if 1
    // calculate gradient dependent on current transfer function
    // using central differences
    vec3 grad;

    float nx  = color_lookup(texture3D(_volume, sample_pos - vec3(_voxel_size.x, 0.0, 0.0))).a;
    float px  = color_lookup(texture3D(_volume, sample_pos + vec3(_voxel_size.x, 0.0, 0.0))).a;

    float ny  = color_lookup(texture3D(_volume, sample_pos - vec3(0.0, _voxel_size.y, 0.0))).a;
    float py  = color_lookup(texture3D(_volume, sample_pos + vec3(0.0, _voxel_size.y, 0.0))).a;

    float nz  = color_lookup(texture3D(_volume, sample_pos - vec3(0.0, 0.0, _voxel_size.z))).a;
    float pz  = color_lookup(texture3D(_volume, sample_pos + vec3(0.0, 0.0, _voxel_size.z))).a;

    grad.x    = (px - nx) / 2.0;
    grad.y    = (py - ny) / 2.0;
    grad.z    = (pz - nz) / 2.0;
    // end gradient calculation

    // lighting
    vec3 n = normalize(gl_TextureMatrix[0] * vec4(gl_NormalMatrix * -grad, 0.0)).xyz; 
    vec3 l = normalize(gl_TextureMatrix[0] * gl_LightSource[0].position).xyz; // assume parallel light!
    vec3 v = normalize(_view_dir);
    vec3 h = normalize(v + l);

    // !! specular still buggy...
    if (length(n) > 0.1) {
    ret.rgb =  //gl_LightSource[0].ambient.rgb
             + ret.rgb * gl_LightSource[0].diffuse.rgb * (dot(n, l) * 0.5 + 0.5);//max(0.0, dot(n, l));/*
             //+ ret.rgb * gl_LightSource[0].specular.rgb * pow(max(0.0, dot(n, h)), gl_FrontMaterial.shininess);

    }
#endif
    return (ret);
}

bool inside_volume_bounds(const in vec3  position,
                          const in float geom_sample_dist)
{
    vec3    sample_to_cam = vec3(position - _cam_pos);
    
    return (   (dot(sample_to_cam, sample_to_cam) < geom_sample_dist)
    //return (   (length(position - _cam_pos) < geom_sample_dist)
            /*&& all(greaterThanEqual(position, _bounds_min))
            && all(lessThanEqual(position, _bounds_max))
            && all(greaterThan(position * entry_in_front_of_planes, _cross_plane_positions * entry_in_front_of_planes))*/);
}

vec3 calculate_starting_point(const in vec3 ray_step)
{
    vec3 starting_point;

    #ifdef RAYCAST_INSIDE_VOLUME

        // calculate intersection point of ray and near plane as starting point;
        // _vol_entry = intersection of the ray with the near_plane
        float t = (-_near_plane.w - dot(_near_plane.xyz, _cam_pos)) / dot(_near_plane.xyz, _ray_dir);
        starting_point = _cam_pos + t * _ray_dir;

    #else // RAYCAST_INSIDE_VOLUME

        starting_point = _obj_pos;

    #endif // RAYCAST_INSIDE_VOLUME

    // correct first step into the volume, so that it is
    // the first whole numbered multiple of a step greater
    // than the initial entry sampling point
    float initial_steps = 1.0 - fract(length(starting_point) / _step_size);
    starting_point += initial_steps * ray_step;

    return (starting_point);
}

float calculate_fragment_depth_objspc()
{
    float depth = texture2DRect(_geom_depth, gl_FragCoord.xy).x;

    vec4 geom_frag_coord_clpspc = vec4(vec2(2.0) * gl_FragCoord.xy / _screen_dimensions - vec2(1.0),
                                       2.0 * depth - 1.0,
                                       1.0);

    vec4 geom_frag_coord_objspc = gl_TextureMatrix[0] * gl_ModelViewProjectionMatrixInverse * geom_frag_coord_clpspc;

    //debug_col = geom_frag_coord_objspc.xyz / geom_frag_coord_objspc.w;

    vec3 frag_to_cam = vec3((geom_frag_coord_objspc.xyz / geom_frag_coord_objspc.w).xyz - _cam_pos);

    //return (length((geom_frag_coord_objspc.xyz / geom_frag_coord_objspc.w).xyz - _cam_pos));
    return (dot(frag_to_cam, frag_to_cam));
}

void main()
{
    vec4    dst                 = vec4(0.0, 0.0, 0.0, 0.0);
    vec3    volume_ray_step     = _step_size * normalize(_ray_dir);
    vec3    sample_pos          = calculate_starting_point(volume_ray_step);
    float   frag_depth_objspc   = calculate_fragment_depth_objspc();

    // for axis aligned volume cross section rendering
    //entry_in_front_of_planes = vec3((sample_pos > _cross_plane_positions)) * 2.0 - vec3(1.0);

    vec4 src;
    vec4 col;

    for (int k = 0;
            k < 255
         && (dst.a < alpha_termination)
         && inside_volume_bounds(sample_pos, frag_depth_objspc);
         k++)
    {
        for (int i = 0;
                i < 255
             && (dst.a < alpha_termination)
             && inside_volume_bounds(sample_pos, frag_depth_objspc);
             i++)
        {

            // get sample
            src = texture3D(_volume, sample_pos);
            col = do_shading(sample_pos, src);
            col.a = 1.0 - pow(1.0 - col.a, (_step_size/(1.0/768.0)));

            //col.rgb = src.rgb;

            // compositing
            float omda_sa = (1.0 - dst.a)*col.a;
            dst.rgb += omda_sa*col.rgb;
            dst.a   += omda_sa;

            // increment ray
            sample_pos += volume_ray_step;
        }
    }

    // debug bullshit
    //const float zFar  = gl_DepthRange.far;
	//const float zNear = gl_DepthRange.near;

	//float a = zFar / (zFar - zNear);
	//float b = zFar * zNear / (zNear - zFar);

	//float dist = b / (depth - a);

    //gl_FragColor = vec4(dist, dist, dist, 1.0);//vec4(frag_coord, 0.0, 1.0); // 
    //gl_FragColor = vec4(depth, depth, depth, 1.0);//vec4(frag_coord, 0.0, 1.0); // 

    //gl_FragColor = vec4(geom_frag_coord.r, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(0.0, geom_frag_coord.g, 0.0, 1.0);
    //gl_FragColor = vec4(0.0, 0.0, geom_frag_coord.b, 1.0);

    //gl_FragColor = vec4(geom_frag_coord_objspc.r, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(0.0, geom_frag_coord_objspc.g, 0.0, 1.0);
    //gl_FragColor = vec4(0.0, 0.0, geom_frag_coord_objspc.b, 1.0);
    //gl_FragColor = vec4(geom_frag_coord_objspc.rgb / geom_frag_coord_objspc.w, 1.0);

    //gl_FragColor = vec4(geom_frag_coord.r, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(frag_depth_objspc, frag_depth_objspc, frag_depth_objspc, 1.0);
    //gl_FragColor = vec4(debug_col, 1.0);


// for debugging purposes
//#ifdef RAYCAST_INSIDE_VOLUME
//    dst.rgb = vec3(dot(dst.rgb, lumi_convert));
//    gl_FragDepth = 1.0;
//#endif // RAYCAST_INSIDE_VOLUME
// end debugging

    gl_FragColor = dst;
}