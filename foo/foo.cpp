#include <sophus/se3.hpp>
#include <fmt/core.h>

int main(int argc, char **argv) {

	Sophus::SE3f pose;

	fmt::print("The pose is [{}, {}]\n", pose.so3().unit_quaternion(), pose.translation().transpose());

	return 0;

}
