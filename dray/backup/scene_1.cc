
#include "Scene.h"
#include "backgrounds/Background.h"
#include "cameras/Camera.h"
#include "HitRecord.h"
#include "utils/Image.h"
#include "utils/Math.h"
#include "lights/Light.h"
#include "materials/Material.h"
#include "objects/Object.h"
#include "Ray.h"
#include "RenderContext.h"
#include "sampler/Sampler.h"
#include "filters/Filter.h"
#include <float.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

Scene::Scene()
{
  object = 0;
  background = 0;
  camera = 0;
  ambient = Color(0, 0, 0);
  image = 0;
  minAttenuation = 0;
}

Scene::~Scene()
{
  delete object;
  delete background;
  delete camera;
  delete image;
	delete sampler;
	delete filter;
  for(int i=0;i<static_cast<int>(lights.size());i++){
    Light* light = lights[i];
    delete light;
  }
}

void Scene::preprocess()
{
  background->preprocess();
  for(int i=0;i<static_cast<int>(lights.size());i++){
    Light* light = lights[i];
    light->preprocess();
  }
  double aspect_ratio = image->aspect_ratio();
  camera->preprocess(aspect_ratio);
  object->preprocess();
	
	samplesPerPixel = sampler->getSamplesPerPixel();
}

void Scene::render()
{
  if(!object || !background || !camera || !image){
    cerr << "Incomplete scene, cannot render!\n";
    exit(1);
  }
	cout<<samplesPerPixel << endl;
  int xres = image->getXresolution();
  int yres = image->getYresolution();
  RenderContext context(this);
  double dx = 2./xres;
	int sqrtSpp = (int)sqrt(samplesPerPixel);
  double xmin = -1. + dx/(sqrtSpp*2.0);
  double dy = 2./yres;
  double ymin = -1. + dy/(sqrtSpp*2.0);
  Color atten(1,1,1);
	int support = filter->getSupport();
	
	double *weight = (double *)calloc(xres*yres, sizeof(double));
	Color *buffer  = new Color[xres*yres];
    for (int idx = 0 ; idx < xres*yres ; idx++) 
		buffer[idx].zero();	
	
  for(int i=0;i<yres;i++){
    //cerr << "y=" << i << '\n';
    
	 //double y = i - 0.5 * yres;
    for(int j=0;j<xres;j++){
		//double x = j - 0.5 * xres;
      
      //cerr << "x=" << j << ", y=" << i << ": " << x << " " << y << '\n';
		Color result(0.0,0.0,0.0);
		double totalWeight = 0.0;
		for(int k = 0; k < samplesPerPixel; k++) {
			//for(int l = 0; l < filter->getSupport(); l++ ) {
			Point sample = sampler->getNextSample();
			double samplex = sample.x(), sampley = sample.y();
			//cout << samplex << " " << sampley << endl;
			double x = xmin + j*dx + dx*samplex;
			double y = ymin + i*dy + dy*sampley;
			//cerr << "x=" << j << ", y=" << i << ": " << x << " " << y << '\n';
			Ray ray;
			camera->makeRay(ray, context, x, y);
			HitRecord hit(DBL_MAX);
			object->intersect(hit, context, ray);
			Color sampleColor;
			if(hit.getPrimitive()){
				// Ray hit something...
				const Material* matl = hit.getMaterial();
				matl->shade(sampleColor, context, ray, hit, atten, 0);
				//matl->shade(result, context, ray, hit, atten, 0);
			} else {
				background->getBackgroundColor(sampleColor, context, ray);
			}
			double colorWeight = filter->evaluate( samplex, sampley); //+ sample.z(), sampley + sample.z());
			result += sampleColor*colorWeight;
			totalWeight += colorWeight;
				//cout << colorWeight << endl;
			//}
		    // apply the values just computed to all the pixels
		    // where it contributes
		    float z;

/*		    for (int dt= -support/2 ; dt <= support/2 ; dt++) {
				int pixel_y = j+dt;
				if (pixel_y >= 0 && pixel_y < yres) {
				    for (int ds= -support/2 ; ds <= support/2 ; ds++) {
						int pixel_x = i+ds;
						if (pixel_x >= 0 && pixel_x < xres) {
							// find the center of this pixel
							double yctr = -1.0 + (pixel_y+0.5)*dy;
							double xctr = -1.0 + (pixel_x+0.5)*dx;

							z = (ds + (dt*support)) * dx * 2;

							//double wt = filter->evaluate(xctr, yctr, xx, yy, dx, dy);
							double e1 = Abs(xctr-x);
							e1 = Clamp(e1, 0.0, 1.0);
							double wt1 = 1.0 - e1;
							wt1 = Clamp(wt1, 0.0, 1.0);
							double e2 = Abs(yctr-y);
							e2 = Clamp(e2, 0.0, 1.0);
							double wt2 = 1.0 - e2;
							wt2 = Clamp(wt2, 0.0, 1.0);
							
							double wt = wt1 * wt2;
							buffer[pixel_x+(pixel_y*xres)] = buffer[pixel_x+(pixel_y*xres)] + result * wt;
							weight[pixel_x+(pixel_y*xres)] += wt;
						}
			    	}
				}
		    }*/
		}
      image->set(j, i, result/totalWeight);
    }
  }
	   /* for (int j=0 ; j < yres ; j++) {	for (int i=0 ; i < xres ; i++) {
	    buffer[i+(j*xres)] = buffer[i+(j*xres)] / weight[i+(j*xres)];
	    image->set(i, j, buffer[i+(j*xres)]);
	}
    }

    delete[]  buffer;*/
}

double Scene::traceRay(Color& result, const RenderContext& context, const Ray& ray, const Color& atten, int depth) const
{
  if(depth >= maxRayDepth || atten.maxComponent() < minAttenuation){
    result = Color(0, 0, 0);
    return 0;
  } else {
    HitRecord hit(DBL_MAX);
    object->intersect(hit, context, ray);
    if(hit.getPrimitive()){
      // Ray hit something...
      const Material* matl = hit.getMaterial();
      matl->shade(result, context, ray, hit, atten, depth);
      return hit.minT();
    } else {
      background->getBackgroundColor(result, context, ray);
      return DBL_MAX;
    }
  }
}

double Scene::traceRay(Color& result, const RenderContext& context, const Object* obj, const Ray& ray, const Color& atten, int depth) const
{
  if(depth >= maxRayDepth || atten.maxComponent() < minAttenuation){
    result = Color(0, 0, 0);
    return 0;
  } else {
    HitRecord hit(DBL_MAX);
    obj->intersect(hit, context, ray);
    if(hit.getPrimitive()){
      // Ray hit something...
      const Material* matl = hit.getMaterial();
      matl->shade(result, context, ray, hit, atten, depth);
      return hit.minT();
    } else {
      background->getBackgroundColor(result, context, ray);
      return DBL_MAX;
    }
  }
}

