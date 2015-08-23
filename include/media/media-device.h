/*
 * Media device
 *
 * Copyright (C) 2010 Nokia Corporation
 *
 * Contacts: Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 *	     Sakari Ailus <sakari.ailus@iki.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _MEDIA_DEVICE_H
#define _MEDIA_DEVICE_H

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>

#include <media/media-devnode.h>
#include <media/media-entity.h>

struct device;

/**
 * struct media_device - Media device
 * @dev:	Parent device
 * @devnode:	Media device node
 * @model:	Device model name
 * @serial:	Device serial number (optional)
 * @bus_info:	Unique and stable device location identifier
 * @hw_revision: Hardware device revision
 * @driver_version: Device driver version
 * @topology_version: Monotonic counter for storing the version of the graph
 *		topology. Should be incremented each time the topology changes.
 * @entity_id:	Unique ID used on the last entity registered
 * @pad_id:	Unique ID used on the last pad registered
 * @link_id:	Unique ID used on the last link registered
 * @intf_devnode_id: Unique ID used on the last interface devnode registered
 * @entities:	List of registered entities
 * @interfaces:	List of registered interfaces
 * @pads:	List of registered pads
 * @links:	List of registered links
 * @lock:	Entities list lock
 * @graph_mutex: Entities graph operation lock
 * @link_notify: Link state change notification callback
 *
 * This structure represents an abstract high-level media device. It allows easy
 * access to entities and provides basic media device-level support. The
 * structure can be allocated directly or embedded in a larger structure.
 *
 * The parent @dev is a physical device. It must be set before registering the
 * media device.
 *
 * @model is a descriptive model name exported through sysfs. It doesn't have to
 * be unique.
 */
struct media_device {
	/* dev->driver_data points to this struct. */
	struct device *dev;
	struct media_devnode devnode;

	char model[32];
	char serial[40];
	char bus_info[32];
	u32 hw_revision;
	u32 driver_version;

	u32 topology_version;

	u32 entity_id;
	u32 pad_id;
	u32 link_id;
	u32 intf_devnode_id;

	struct list_head entities;
	struct list_head interfaces;
	struct list_head pads;
	struct list_head links;

	/* Protects the entities list */
	spinlock_t lock;
	/* Serializes graph operations. */
	struct mutex graph_mutex;

	int (*link_notify)(struct media_link *link, u32 flags,
			   unsigned int notification);
};

#ifdef CONFIG_MEDIA_CONTROLLER

/* Supported link_notify @notification values. */
#define MEDIA_DEV_NOTIFY_PRE_LINK_CH	0
#define MEDIA_DEV_NOTIFY_POST_LINK_CH	1

/* media_devnode to media_device */
#define to_media_device(node) container_of(node, struct media_device, devnode)

int __must_check __media_device_register(struct media_device *mdev,
					 struct module *owner);
#define media_device_register(mdev) __media_device_register(mdev, THIS_MODULE)
void media_device_unregister(struct media_device *mdev);

int __must_check media_device_register_entity(struct media_device *mdev,
					      struct media_entity *entity);
void media_device_unregister_entity(struct media_entity *entity);
struct media_device *media_device_get_devres(struct device *dev);
struct media_device *media_device_find_devres(struct device *dev);

/* Iterate over all entities. */
#define media_device_for_each_entity(entity, mdev)			\
	list_for_each_entry(entity, &(mdev)->entities, graph_obj.list)

/* Iterate over all interfaces. */
#define media_device_for_each_intf(intf, mdev)			\
	list_for_each_entry(intf, &(mdev)->interfaces, graph_obj.list)

/* Iterate over all pads. */
#define media_device_for_each_pad(pad, mdev)			\
	list_for_each_entry(pad, &(mdev)->pads, graph_obj.list)

/* Iterate over all links. */
#define media_device_for_each_link(link, mdev)			\
	list_for_each_entry(link, &(mdev)->links, graph_obj.list)


#else
static inline int media_device_register(struct media_device *mdev)
{
	return 0;
}
static inline void media_device_unregister(struct media_device *mdev)
{
}
static inline int media_device_register_entity(struct media_device *mdev,
						struct media_entity *entity)
{
	return 0;
}
static inline void media_device_unregister_entity(struct media_entity *entity)
{
}
static inline struct media_device *media_device_get_devres(struct device *dev)
{
	return NULL;
}
static inline struct media_device *media_device_find_devres(struct device *dev)
{
	return NULL;
}
#endif /* CONFIG_MEDIA_CONTROLLER */
#endif
