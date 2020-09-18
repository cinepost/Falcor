#include "grammar_lsd.h"

namespace x3 = boost::spirit::x3;
namespace fs = boost::filesystem;

namespace lava { 

namespace lsd {

void LSDEchoVisitor::operator()(int v) const {
    _os << v;
}

void LSDEchoVisitor::operator()(double v) const {
    _os << v;
}

void LSDEchoVisitor::operator()(std::string const& v) const {
    _os << v;
}

void LSDEchoVisitor::operator()(Int2 const& v) const {

}

void LSDEchoVisitor::operator()(Int3 const& v) const {

}

void LSDEchoVisitor::operator()(Int4 const& v) const {

}

void LSDEchoVisitor::operator()(Vector2 const& v) const {

}

void LSDEchoVisitor::operator()(Vector3 const& v) const {

}

void LSDEchoVisitor::operator()(Vector4 const& v) const {

}

void LSDEchoVisitor::operator()(PropValue const& v) const { 
    operator()(v);
}

void LSDEchoVisitor::operator()(std::vector<PropValue> const& v) const {
    for(std::vector<PropValue>::const_iterator it = v.begin(); it != v.end(); it++) {
        operator()(*it);
        _os << " ";
    }
}

void LSDEchoVisitor::operator()(ast::setenv const& c) const { 
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> setenv: " << c.key << " = " << c.value << "\x1b[0m\n"; 
};

void LSDEchoVisitor::operator()(ast::cmd_image const& c) const { 
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_image: " << c.values << "\x1b[0m\n"; 
}

void LSDEchoVisitor::operator()(ast::cmd_end const& c) const { 
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_end: " << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_quit const& c) const { 
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_quit: " << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_start const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_start: " << c.type << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_time const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_time: " << c.time << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_detail const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_detail: name: " << c.name << " filename: " << c.filename << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_detail_inline const& c) const { 
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_detail (inline): name: " << c.name << " stdin\n"; 
    boost::apply_visitor(bgeo::EchoVisitor(), c.bgeo);
    _os << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_version const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_version: " << c.version << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_defaults const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_defaults: filename: " << c.filename << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_transform const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_transform: " << c.m << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_geometry const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_geometry: geometry_object: " << c.geometry_object << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_property const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_property: style: " << c.style << " token: " << c.token << " values: "<< c.values.size() << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_deviceoption const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_deviceoption: type: " << c.type << " name: " << c.name << " values: "<< c.values << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_declare const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_declare: style: " << c.style << " token: " << c.token << " type: " << c.type << " values: ";
    //operator()(c.values);
    _os << "\x1b[0m\n";
}

void LSDEchoVisitor::operator()(ast::cmd_raytrace const& c) const {
    LSDVisitor::operator()(c);
    _os << "\x1b[32m" << "> cmd_raytrace: " << "\x1b[0m\n";
}

}  // namespace lsd

}  // namespace lava
