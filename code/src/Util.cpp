#include "Util.h"

#include "glinc.h"



namespace Util {
    double lerp(double a, double b, double t) {
        return a + ((b - a) * t);
    }



    glm::vec3 lerp3D(glm::vec3 a, glm::vec3 b, double t) {
        glm::vec3 ret;
        ret.x = a.x + ((b.x - a.x) * t);
        ret.y = a.y + ((b.y - a.y) * t);
        ret.z = a.z + ((b.z - a.z) * t);
        return ret;
    }



    bool NodeCompare(node* a, node* b) {
        bool ret;
        if (a->entropy > b->entropy) {
            ret = true;
        } else {
            ret = false;
        }
        return ret;
    }
}