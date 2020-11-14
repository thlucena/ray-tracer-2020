#ifndef _BLINN_PHONG_INTEGRATOR_
#define _BLINN_PHONG_INTEGRATOR_

#include <iostream>
#include "../core/integrator.h"
#include "../core/light.h"
#include "../lights/ambient_light.h"
#include "../materials/blinn_phong_material.h"

class BlinnPhongIntegrator: public SamplerIntegrator {
    private:
        int depth;
    public:
        BlinnPhongIntegrator(Camera * cam, int mDepth) : SamplerIntegrator(cam), depth{ mDepth } { }

        Color Li(const Ray& ray, const Scene & scene, const Color bkg_color ) const override {
            Color L(0,0,0);
            Surfel isect; // Intersection information.  
            if (!scene.intersect(ray, &isect)) {
                L = bkg_color;
            } else {
                BlinnPhongMaterial *bpMaterial = dynamic_cast< BlinnPhongMaterial *>( isect.primitive->getMaterial() );
                auto ka = bpMaterial->ambient;
                auto kd = bpMaterial->diffuse;
                auto ks = bpMaterial->specular;
                auto glossiness = bpMaterial->glossiness;
                auto vecN = isect.n ;  // Normal vector at the point where the ray hit the surface.
                Vec3 ambientLight = Vec3(0, 0, 0);
                Vec3 result = Vec3(0, 0, 0);
                Vec3 vecP = Vec3{isect.p.getX(), isect.p.getY(), isect.p.getZ()};

                for(auto & light : scene.lights) { 
                    if (light->getType() == light_type_e::ambient) {
                        light->sample_Li(isect, &ambientLight);
                        continue;
                    }
                    Vec3 I = Vec3();
                    auto color = light->sample_Li(isect, &I);
                    auto vecL = Vec3(color.r(), color.g(), color.b());
                    if (light->getTypeString() == "directional") {
                        Vec3 vecDirecton = light->getFrom() - vecP;
                        Ray shadow_ray{isect.p, vecDirecton};
                        for(auto & object : scene.objects) {
                            if (object->intersect_p(shadow_ray)) {
                                return L;
                            }
                        }
                    } else {
                        Vec3 vecDirecton = light->getFrom() - vecP;
                        Ray shadow_ray{isect.p, vecDirecton, 0.0, 1.0};
                        // for(auto & object : scene.objects) {               
                            if (scene.intersect_p(shadow_ray)) {
                                return L;
                            }
                       // }
                    }
                    // o h pode ser um l+v/||l+v|| esse valor de h é do material de selan
                    Vec3 vecH = normalize( vecL + normalize( -ray.getDirection() ) ); // nao ta seguindo a recomendação de selan mas sim o material que ele passou referente ao blinnphong
                    float maxValue = max(0.f, dot(vecN, vecH));
                    result += kd * I * max( 0.f, dot( vecN, vecL )) + ks * I * pow(maxValue, glossiness); // light_I é variável de acordo com a luz da vez entao tem que ser algo tipo lights[i].i
                }

                result = ka * ambientLight + result; // coeficientee que ka vem do arquivo de cena assim como a luz ambiente
                float r = result.r() > 1 ? 1.0 : result.r();
                float g = result.g() > 1 ? 1.0 : result.g();
                float b = result.b() > 1 ? 1.0 : result.b();
                L = Color(r, g, b);
            }
            return L;
        }
};

#endif