
#ifndef SCM_DATA_VOLUME_DATA_LOADER_RAW_H_INCLUDED
#define SCM_DATA_VOLUME_DATA_LOADER_RAW_H_INCLUDED

#include <fstream>

#include <scm/data/volume/volume_data_loader.h>

#include <scm/core/platform/platform.h>
#include <scm/core/utilities/platform_warning_disable.h>

namespace scm {
namespace data {

class __scm_export(data) volume_data_loader_raw : public volume_data_loader
{
public:
    volume_data_loader_raw();
    virtual ~volume_data_loader_raw();

    virtual bool        open_file(const std::string& filename);
    virtual bool        open_file(const std::string& filename,
                                  const math::vec3ui_t& dim,
                                  unsigned num_chan,
                                  unsigned byte_per_chan);

    virtual bool        read_sub_volume(const math::vec<unsigned, 3>& offset,
                                        const math::vec<unsigned, 3>& dimensions,
                                        scm::data::regular_grid_data_3d<unsigned char>& target_data); 

protected:

private:

}; // namespace volume_data_loader_raw

} // namespace data
} // namespace scm

#include <scm/core/utilities/platform_warning_enable.h>

#endif // SCM_DATA_VOLUME_DATA_LOADER_RAW_H_INCLUDED