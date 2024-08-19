/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "flowlayout.h"
#include "utils-api.h"
#include "libutils-api.h"

//! [1]
FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing) : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
	setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing) : m_hSpace(hSpacing), m_vSpace(vSpacing)
{
	setContentsMargins(margin, margin, margin, margin);
}
//! [1]

//! [2]
FlowLayout::~FlowLayout()
{
	QLayoutItem *item;
	while ((item = takeAt(0)))
		pls_delete(item);
}
//! [2]

//! [3]
void FlowLayout::addItem(QLayoutItem *item)
{
	itemList.append(item);
	if (retainSize) {
		// hide the widget first(but still retain size) and show after layout finished
		QSizePolicy sizePolicy = item->widget()->sizePolicy();
		sizePolicy.setRetainSizeWhenHidden(true);
		item->widget()->setSizePolicy(sizePolicy);
		item->widget()->setVisible(false);
	}
}
//! [3]

//! [4]
int FlowLayout::horizontalSpacing() const
{
	if (m_hSpace >= 0) {
		return m_hSpace;
	} else {
		return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
	}
}

int FlowLayout::verticalSpacing() const
{
	if (m_vSpace >= 0) {
		return m_vSpace;
	} else {
		return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
	}
}

void FlowLayout::setHorizontalSpacing(int hSpacing)
{
	m_hSpace = hSpacing;
}
void FlowLayout::setverticalSpacing(int vSpacing)
{
	m_vSpace = vSpacing;
}
//! [4]

//! [5]
int FlowLayout::count() const
{
	return (int)itemList.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
	return itemList.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
	if (index >= 0 && index < itemList.size())
		return itemList.takeAt(index);
	else
		return nullptr;
}
void FlowLayout::setItemRetainSizeWhenHidden(bool retainSize_)
{
	retainSize = retainSize_;
}
int FlowLayout::rowForWidth(int width) const
{
	int row = 0;
	doLayout(QRect(0, 0, width, 0), true, &row);
	return row;
}
//! [5]

//! [6]
Qt::Orientations FlowLayout::expandingDirections() const
{
	return {};
}
//! [6]

//! [7]
bool FlowLayout::hasHeightForWidth() const
{
	return true;
}

int FlowLayout::heightForWidth(int width) const
{
	if (m_fixWidth > 0) {
		width = m_fixWidth;
	}
	if (this->parentWidget()) {
		bool finishLayout = width == this->parentWidget()->width();
		if (finishLayout)
			pls_async_call(this, [this]() { emit LayoutFinished(); });
	}
	int height = doLayout(QRect(0, 0, width, 0), true);
	return height;
}
//! [7]

//! [8]
void FlowLayout::setGeometry(const QRect &rect)
{
	QLayout::setGeometry(rect);
	doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
	return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
	QSize size;
	const QLayoutItem *item = nullptr;
	foreach(item, itemList) size = size.expandedTo(item->minimumSize());
	int l;
	int top;
	int r;
	int bottom;
	getContentsMargins(&l, &top, &r, &bottom);
	size += QSize(l + r, top + bottom);
	return size;
}
//! [8]

//! [9]
int FlowLayout::doLayout(const QRect &rect, bool testOnly, int *rowOut) const
{
	int left;
	int top;
	int right;
	int bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
	int x = effectiveRect.x();
	int y = effectiveRect.y();
	int lineHeight = 0;
	//! [9]

	//! [10]
	QLayoutItem *item;
	int row = 0;
	foreach(item, itemList)
	{
		const QWidget *wid = item->widget();
		int spaceX = horizontalSpacing();
		if (spaceX == -1)
			spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
		int spaceY = verticalSpacing();
		if (spaceY == -1)
			spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
		//! [10]
		//! [11]
		int nextX = x + item->sizeHint().width() + spaceX;
		if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
			x = effectiveRect.x();
			y = y + lineHeight + spaceY;
			nextX = x + item->sizeHint().width() + spaceX;
			lineHeight = 0;
			row++;
		}

		if (!testOnly)
			item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

		x = nextX;
		lineHeight = qMax(lineHeight, item->sizeHint().height());
	}
	if (rowOut) {
		if (!itemList.empty())
			*rowOut = ++row;
	}
	return y + lineHeight - rect.y() + bottom;
}
//! [11]
//! [12]
int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
	QObject *parent = this->parent();
	if (!parent) {
		return -1;
	} else if (parent->isWidgetType()) {
		auto pw = static_cast<QWidget *>(parent);
		return pw->style()->pixelMetric(pm, nullptr, pw);
	} else {
		return static_cast<QLayout *>(parent)->spacing();
	}
}
void FlowLayout::showLayoutItemWidget() const
{
	const QLayoutItem *item;
	foreach(item, itemList)
	{
		item->widget()->setVisible(true);
	}
}
//! [12]
