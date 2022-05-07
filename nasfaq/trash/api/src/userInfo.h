#ifndef _USERINFO_H_
#define _USERINFO_H_

typedef struct userInfo_t {

	bool admin;
	float brokerFreeTotal;
	char color[USERINFO_COLOR_SIZE];
	char email[USERINFO_EMAIL_SIZE];
	char hat[USERINFO_HAT_SIZE];
	char icon[USERINFO_ICON_SIZE];
	char id[USERINFO_ICON_SIZE];
	bool isBookie;
	items_t items;
	bool loggedout;
	muted_t muted;
	performance_t performance;
	settings_t settings;
	float taxCredits;
	char username[USERINFO_USERNAME_SIZE];
	bool verified;
	wallet_t wallet;
} userInfo_t;

#endif

