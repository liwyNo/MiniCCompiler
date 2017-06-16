require "AssessmentBase.rb"

module Eeyore
  include AssessmentBase

  def assessmentInitialize(course)
    super("eeyore",course)
    @problems = []
  end

end
