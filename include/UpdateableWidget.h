/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_UPDATEABLE_WIDGET_H_
#define INCLUDE_UPDATEABLE_WIDGET_H_

#include "User.h"

class UpdateableWidget {

  public:

    UpdateableWidget() {}

    virtual ~UpdateableWidget() {}

    virtual void update() = 0;

};

#endif // INCLUDE_UPDATEABLE_WIDGET_H_
