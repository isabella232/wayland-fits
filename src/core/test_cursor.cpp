#include <boost/bind.hpp>
#include <wayland-cursor.h>
#include "harness.h"

/** This is as defined in wayland/cursor/wayland-cursor.c **/
struct wl_cursor_theme {
	unsigned int cursor_count;
	struct wl_cursor **cursors;
	struct wl_shm *shm;
	struct shm_pool *pool;
	char *name;
	int size;
};

class ThemeLoadTest : public CoreTestHarness
{
public:
	ThemeLoadTest()
		: CoreTestHarness::CoreTestHarness()
		, shm_(0)
	{
		return;
	}

	void handleGlobal(uint32_t id, const std::string& iface, uint32_t version)
	{
		if (iface == "wl_shm")
		{
			shm_ = static_cast<wl_shm*>(wl_registry_bind(*this, id, &wl_shm_interface, 1));
			FAIL_IF_EQUAL(shm_, NULL);
		}
	}

	void setup()
	{
		queueTest(
			boost::bind(&ThemeLoadTest::test, boost::ref(*this), "default", "default")
		);
		queueTest(
			boost::bind(&ThemeLoadTest::test, boost::ref(*this), "this_theme_does_not_exist", "default")
		);
	}
	
	void test(const std::string load, const std::string expect)
	{
		wl_cursor_theme* theme(
			wl_cursor_theme_load(load.c_str(), 32, shm_)
		);
		FAIL_IF_EQUAL(theme, NULL);
		FAIL_UNLESS(expect == std::string(theme->name));

		wl_cursor_theme_destroy(theme);
	}

private:
	wl_shm*	shm_;
};

WFITS_CORE_HARNESS_TEST_CASE(ThemeLoadTest, "Cursor");
