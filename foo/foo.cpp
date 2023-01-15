#include <sophus/se3.hpp>
#include <fmt/core.h>

#include <linear.h>

Sophus::SE3f r3ToSophus(const r3::Posef& p) {
	return *reinterpret_cast<const Sophus::SE3f*>(&p);
}

int main(int argc, char **argv) {

	Sophus::SE3f pose;
	r3::Posef p2(r3::Quaternionf(r3::Vec3f(0, 1, 0), r3::ToRadians(45.0f)), r3::Vec3f(0, 0, -1));
	pose = r3ToSophus(p2);

	fmt::print("The pose is [{}, {}]\n", pose.so3().unit_quaternion(), pose.translation().transpose());

	return 0;

}
