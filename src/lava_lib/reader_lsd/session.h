#ifndef SRC_LAVA_LIB_READER_LSD_SESSION_H_
#define SRC_LAVA_LIB_READER_LSD_SESSION_H_

#include <memory>

#include "grammar_lsd.h"
#include "../reader_bgeo/bgeo/Bgeo.h"
#include "../renderer_iface.h"
#include "scope.h"

#include "../scene_builder.h"

namespace lava {

namespace lsd {

class Session {
 public:
 	using UniquePtr = std::unique_ptr<Session>;
    static UniquePtr create(std::unique_ptr<RendererIface> pRendererIface);
    
    ~Session();

 public:
 	scope::Geo::SharedPtr getCurrentGeo();

 	bool cmdStart(lsd::ast::Style object_type);
 	bool cmdEnd();
 	void cmdSetEnv(const std::string& key, const std::string& value);
 	bool cmdRaytrace();
    void cmdConfig(const std::string& file_name);
    void cmdProperty(lsd::ast::Style style, const std::string& token, const lsd::PropValue& value);
    void cmdDeclare(lsd::ast::Style style, lsd::ast::Type type, const std::string& token, const lsd::PropValue& value);
    bool cmdImage(lsd::ast::DisplayType display_type, const std::string& filename);
    void cmdTransform(const Matrix4& transform);

    void pushBgeo(const std::string& name, ika::bgeo::Bgeo& bgeo);
    std::string getExpandedString(const std::string& str);

 private:
 	Session(std::unique_ptr<RendererIface> pRendererIface);
 	
 	bool initRenderData();
 	bool setDisplayByType(const lsd::ast::DisplayType& display_type);
 	bool setDisplayByFileName(const std::string& file_name);

 	bool pushScriptFiles();

 private:
 	std::unique_ptr<RendererIface> 	mpRendererIface;
 	std::vector<std::string> 		mGraphConfigsFileNames;
 	RendererIface::FrameData		mFrameData;

 	scope::ScopeBase::SharedPtr		mpCurrentScope;
 	scope::Global::SharedPtr		mpGlobal;

 	std::map<std::string, uint32_t>	mMeshMap; // maps detail(mesh) name to SceneBuilder mesh id	
};

}  // namespace lsd

}  // namespace lava

#endif  // SRC_LAVA_LIB_READER_LSD_SESSION_H_