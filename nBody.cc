#include <nan.h>
#include <math.h>
#include "nBody.h"

// native node addon module inspired by the example/async_pi_estimate at
// https://github.com/nodejs/nan

using v8::Function;
using v8::Local;
using v8::Number;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;

class Body {
    public :
        constexpr static double PI = 3.141592653589793;
        constexpr static double SOLAR_MASS = 4 * PI * PI;
        constexpr static double DAYS_PER_YEAR = 365.24;
        double x, y, z, vx, vy, vz, mass;

        Body(
            double X,
            double Y,
            double Z,
            double vX,
            double vY,
            double vZ,
            double Mass
        ): x(X), y(Y), z(Z), vx(vX), vy(vY), vz(vZ), mass(Mass)
        {}
        
        void offsetMomentum(double px, double py, double pz) {
            vx = -px / Body::SOLAR_MASS;
            vy = -py / Body::SOLAR_MASS;
            vz = -pz / Body::SOLAR_MASS;
        }
};

class NBodySystem {
public:
    
    Body** bodies;

    int bodyLength;

    NBodySystem(): bodyLength(5) {
        bodies = new Body* [bodyLength];
        // sun
        bodies[0] = new Body(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Body::SOLAR_MASS);
        // jupiter
        bodies[1] = new Body(
            4.84143144246472090e+00,
            -1.16032004402742839e+00,
            -1.03622044471123109e-01,
            1.66007664274403694e-03 * Body::DAYS_PER_YEAR,
            7.69901118419740425e-03 * Body::DAYS_PER_YEAR,
            -6.90460016972063023e-05 * Body::DAYS_PER_YEAR,
            9.54791938424326609e-04 * Body::SOLAR_MASS
        );
        // saturn
        bodies[2] = new Body(
            8.34336671824457987e+00,
            4.12479856412430479e+00,
            -4.03523417114321381e-01,
            -2.76742510726862411e-03 * Body::DAYS_PER_YEAR,
            4.99852801234917238e-03 * Body::DAYS_PER_YEAR,
            2.30417297573763929e-05 * Body::DAYS_PER_YEAR,
            2.85885980666130812e-04 * Body::SOLAR_MASS
        );
        // uranus
        bodies[3] = new Body(
            1.28943695621391310e+01,
            -1.51111514016986312e+01,
            -2.23307578892655734e-01,
            2.96460137564761618e-03 * Body::DAYS_PER_YEAR,
            2.37847173959480950e-03 * Body::DAYS_PER_YEAR,
            -2.96589568540237556e-05 * Body::DAYS_PER_YEAR,
            4.36624404335156298e-05 * Body::SOLAR_MASS
        );
        // neptune
        bodies[4] = new Body(
            1.53796971148509165e+01,
            -2.59193146099879641e+01,
            1.79258772950371181e-01,
            2.68067772490389322e-03 * Body::DAYS_PER_YEAR,
            1.62824170038242295e-03 * Body::DAYS_PER_YEAR,
            -9.51592254519715870e-05 * Body::DAYS_PER_YEAR,
            5.15138902046611451e-05 * Body::SOLAR_MASS
        );

        double px = 0.0;
        double py = 0.0;
        double pz = 0.0;
        for(int i=0; i < bodyLength; ++i) {
            px += bodies[i]->vx * bodies[i]->mass;
            py += bodies[i]->vy * bodies[i]->mass;
            pz += bodies[i]->vz * bodies[i]->mass;
        }
        bodies[0]->offsetMomentum(px,py,pz);
    }

    ~NBodySystem() {
        delete [] bodies;
    }

    void advance(double dt) {
        for (int i=0; i < bodyLength; ++i) {
            Body* p = bodies[i];
            for (int j=i+1; j < bodyLength; ++j) {
                double dx = p->x - bodies[j]->x;
                double dy = p->y - bodies[j]->y;
                double dz = p->z - bodies[j]->z;

                double dSquared = dx * dx + dy * dy + dz * dz;
                double distance = sqrt(dSquared);
                double mag = dt / (dSquared * distance);

                p->vx -= dx * bodies[j]->mass * mag;
                p->vy -= dy * bodies[j]->mass * mag;
                p->vz -= dz * bodies[j]->mass * mag;

                bodies[j]->vx += dx * p->mass * mag;
                bodies[j]->vy += dy * p->mass * mag;
                bodies[j]->vz += dz * p->mass * mag;
            }
        }

        for (int j=0; j < bodyLength; ++j) {
            bodies[j]->x += dt * bodies[j]->vx;
            bodies[j]->y += dt * bodies[j]->vy;
            bodies[j]->z += dt * bodies[j]->vz;
        }
    }

   double energy() {
      double dx, dy, dz, distance;
      double e = 0.0;

      for (int i=0; i < bodyLength; ++i) {
         Body* p = bodies[i];
         e += 0.5 * p->mass *
            ( p->vx * p->vx
                + p->vy * p->vy
                + p->vz * p->vz );

         for (int j=i+1; j < bodyLength; ++j) {
            Body* q = bodies[j];
            dx = p->x - q->x;
            dy = p->y - q->y;
            dz = p->z - q->z;

            distance = sqrt(dx*dx + dy*dy + dz*dz);
            e -= (p->mass * q->mass) / distance;
         }
      }
      return e;
   }
};

class NBodyWorker : public AsyncWorker {
 public:
  NBodyWorker(Callback *callback, int Count)
    : AsyncWorker(callback), count(Count) {
    }
  ~NBodyWorker() {}

  // Executed inside the worker-thread.
  void Execute () {
    for (int i = 0; i < count; i++) {
        n.advance(0.01);
    }
  }

  // Executed when the async work is complete to send the result
  void HandleOKCallback () {
    HandleScope scope;

    Local<Value> argv[] = {
        Null()
      , New<Number>(energy())
    };

    callback->Call(2, argv);
  }

  // access to the energy()
  double energy() {
      return n.energy();
  }

 private:
  int count;
  NBodySystem n;

};

// Asynchronous access to nBody calculation for count
NAN_METHOD(nBody) {
    int count = To<int>(info[0]).FromJust();
    Callback *callback = new Callback(info[1].As<Function>());

    NBodyWorker* pN = new NBodyWorker(callback, count);
    
    // send an initial result
    Local<Value> argv[] = {
        Null(),
        New<Number>(pN->energy())
    };
    
    callback->Call(2, argv);

    AsyncQueueWorker(pN);
}