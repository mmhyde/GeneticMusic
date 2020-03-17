#pragma once

namespace Genetics {

	class UIElement {

	public:

		UIElement() {}
		virtual ~UIElement() {}

		virtual void render() = 0;

	private:


	};


} // namespace Genetics